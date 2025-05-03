#include "Logger.h"

#include "AppInfo.h"

#include "FolderLocationManager.h"
#include "CrashDetector.h"

static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(nullptr);

Logger::Logger(QObject *parent) : QObject(parent) {
    // get logfile dir
    QDir logDir = FolderLocationManager::currentFolderLocationPaths("logfile").first();

    // check if crash was detected. In this case copy logfile
    if(CrashDetector::crashDetected()) {
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
    f.open(QIODevice::WriteOnly | QIODevice::Text);
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

    qInstallMessageHandler(Logger::handler);
}

void Logger::handler(QtMsgType type, const QMessageLogContext &context, const QString &message) {
    if(type == QtMsgType::QtDebugMsg && _logfileMode != DebugLog && _logfileMode != DebugDetailLog)
        return;

    QFile f(_logfilePath);
    f.open(QIODevice::Append | QIODevice::Text);
    QTextStream s(&f);


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

    QString identation("   ");
    identation = identation.repeated(_subCount);

    QString prefix("[%1] ");
    prefix = _prefix.isEmpty() ? "" : prefix.arg(_prefix);

    if(_logfileMode == DebugDetailLog) {
        s << "       |          |                     | " << identation << prefix << message << "\n";

        s << counterStr << " | " << timeStr << " | " << typeStr << " | ";
        s << "in " << context.file << " (Line " << context.line << ")\n";

        s << "       |          |                     | " << context.function << "\n\n";
    } else {
        s << counterStr << " | " << timeStr << " | " << typeStr << " | " << identation << prefix << message << "\n";
    }

    f.close();

    (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, identation + prefix + message);
    _counter++;
}

void Logger::setPrefix(const QString &prefix) {
    _prefix = prefix;
}

void Logger::resetPrefix() {
    _prefix.clear();
}

void Logger::beginSub() {
    _subCount++;
}

void Logger::endSub() {
    _subCount = std::max(0, _subCount - 1);
}

void Logger::setLogfileMode(const LogfileMode &newLogfileMode) {
    _logfileMode = newLogfileMode;
    SettingsManager::setValue("general.logfileMode", newLogfileMode);
}

bool Logger::currentLogfileExists() {
    return !_logfilePath.isEmpty();
}

QString Logger::lastLogfilePath() {
    return _lastLogfilePath;
}

Logger::LogfileMode Logger::logfileMode() {
    bool ok;
    int intValue = SettingsManager::value("general.logfileMode").toInt(&ok);
    if(!ok || intValue < 0 || intValue > 3)
        return DefaultLog;

    const LogfileMode mode = static_cast<LogfileMode>(intValue);
    _logfileMode = mode;
    return mode;
}
