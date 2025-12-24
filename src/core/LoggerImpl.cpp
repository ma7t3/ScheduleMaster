#include "LoggerImpl.h"

#include "src/namespace.h"
#include "src/core/FolderLocationServiceImpl.h"
#include "src/core/CrashDetectorImpl.h"
#include "src/core/SettingsServiceImpl.h"

#include "Global/AppInfo.h"

#include <QDir>
#include <QFile>

static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(nullptr);

namespace ScheduleMaster::Core {

LoggerImpl::LoggerImpl(QObject *parent) : QObject(parent) {
    _instance = this;

    // get logfile dir
    QDir logDir = FolderLocationServiceImpl::instance()->currentFolderLocationPaths("logfile").first();

    // check if crash was detected. In this case copy logfile
    if(CrashDetectorImpl::instance()->crashDetected()) {
        const QDateTime now(QDateTime::currentDateTime());
        QString newFileName = logDir.path() + "/logfile_crash_" + now.toString("yyyy-MM-dd_hh-mm-ss") + ".txt";
        if(QFile::copy(logDir.path() + "/logfile.txt", newFileName))
            _lastLogfilePath = newFileName;
    }

    _logfileMode = logfileMode();

    if(_logfileMode == NoLog)
        return;

    if(!logDir.exists())
        logDir.mkpath(logDir.path());

    _logfilePath = logDir.path() + "/logfile.txt";
    _counter = 0;

    QFile f(_logfilePath);
    if(f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream s(&f);

        QDateTime now = QDateTime::currentDateTime();

        QString logfileModeInfo;
        switch (_logfileMode) {
        case DefaultLog:     logfileModeInfo = "Normal logging";         break;
        case DebugLog:       logfileModeInfo = "Debug logging";          break;
        case DebugDetailLog: logfileModeInfo = "Detailed debug logging"; break;
        default:             logfileModeInfo = "Unkown loggin mode";     break;
        }

        s << "##########################################################################################\n";
        s << "   ScheduleMaster | " << AppInfo::currentVersionName() << " | " << "Qt " << qVersion() << " | " << logfileModeInfo << "\n";
        s << "##########################################################################################\n";
        s << "Time:               " << now.toString("yyyy-MM-dd") << ", " << now.toString("hh:mm:ss") << "\n";
        s << "------------------------------------------------------------------------------------------\n";
        s << "Build information:  " << QSysInfo::buildAbi() << "\n";
        s << "------------------------------------------------------------------------------------------\n";
        s << "System Information: OS:               " << QSysInfo::prettyProductName() << "\n";
        s << "                    Kernel:           " << QSysInfo::kernelType() << " (" << QSysInfo::kernelVersion() << ")\n";
        s << "                    CPU architecture: " << QSysInfo::currentCpuArchitecture() << "\n";
        s << "------------------------------------------------------------------------------------------\n";
        s << "No.    | Time     | Type                | Message\n";

        f.close();
    }
    qInstallMessageHandler(LoggerImpl::messageHandler);
}

void LoggerImpl::messageHandler(QtMsgType type, const QMessageLogContext &context,
                                const QString &message) {
    _instance->log(type, context, message);
}

void LoggerImpl::log(const QtMsgType &type, const QMessageLogContext &context,
                                           const QString &message) {
    if(type == QtMsgType::QtDebugMsg && _logfileMode != DebugLog && _logfileMode != DebugDetailLog)
        return;

    QString typeStr;
    switch (type) {
    case QtMsgType::QtDebugMsg:    typeStr = "            [debug]"; break;
    case QtMsgType::QtInfoMsg:     typeStr = "         [info]    "; break;
    case QtMsgType::QtWarningMsg:  typeStr = "      [warning]    "; break;
    case QtMsgType::QtCriticalMsg: typeStr = "   [critical]      "; break;
    case QtMsgType::QtFatalMsg:    typeStr = "[fatal]            "; break;
    default:                       typeStr = "[unknown]          "; break;
    }

    QString counterStr = QVariant(_counter).toString() + ".";
    while(counterStr.length() < 6)
        counterStr += " ";

    QTime now = QTime::currentTime();
    QString timeStr = now.toString("hh:mm:ss");

    QString prefix("[%1] ");
    prefix = _prefix.isEmpty() ? "" : prefix.arg(_prefix);

    QFile f(_logfilePath);
    if(f.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream s(&f);

        if(_logfileMode == DebugDetailLog) {
            s << "       |          |                     | " << prefix << message << "\n";

            s << counterStr << " | " << timeStr << " | " << typeStr << " | ";
            s << "in " << context.file << " (Line " << context.line << ")\n";

            s << "       |          |                     | " << context.function << "\n\n";
        } else {
            s << counterStr << " | " << timeStr << " | " << typeStr << " | " << prefix << message << "\n";
        }

        f.close();
    }

#ifdef QT_DEBUG
    (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, prefix + message);
#endif
    _counter++;
}

void LoggerImpl::setLogfileMode(const LogfileMode &newLogfileMode) {
    _logfileMode = newLogfileMode;
    SM::SettingsServiceImpl::instance()->setValue("general.logfileMode", newLogfileMode);
}

bool LoggerImpl::currentLogfileExists() const {
    return !_logfilePath.isEmpty();
}

QString LoggerImpl::lastLogfilePath() const {
    return _lastLogfilePath;
}

ILogger::LogfileMode LoggerImpl::logfileMode() const {
    bool ok;
    int intValue = SM::SettingsServiceImpl::instance()->value("general.logfileMode").toInt(&ok);
    if(!ok || intValue < 0 || intValue > 3)
        return DefaultLog;

    return static_cast<LogfileMode>(intValue);
}

}
