#include "Logger.h"

#include "AppInfo.h"

static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(nullptr);

Logger::Logger(QObject *parent, const QDir &logfilesDirectory) : QObject(parent) {
    //logfileMode = LocalConfig::DebugLog; // use this to always enable debug logging - helpful if you broke the LocalConfig and need logging to fix it ^^
    _logfileMode = LocalConfig::logfileMode();

    if(_logfileMode == LocalConfig::NoLog)
        return;

    if(!logfilesDirectory.exists())
        logfilesDirectory.mkpath(logfilesDirectory.path());

    _logfilePath = logfilesDirectory.path() + "/logfile.txt";
    _counter = 0;

    QFile f(_logfilePath);
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream s(&f);

    QDateTime now = QDateTime::currentDateTime();

    QString logfileModeInfo;
    switch (_logfileMode) {
        case LocalConfig::DefaultLog:     logfileModeInfo = "Normal logging";         break;
        case LocalConfig::DebugLog:       logfileModeInfo = "Debug logging";          break;
        case LocalConfig::DebugDetailLog: logfileModeInfo = "Detailed debug logging"; break;
        default:                          logfileModeInfo = "Unkown loggin mode";     break;
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
    if(type == QtMsgType::QtDebugMsg && _logfileMode != LocalConfig::DebugLog && _logfileMode != LocalConfig::DebugDetailLog)
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

    if(_logfileMode == LocalConfig::DebugDetailLog) {
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
