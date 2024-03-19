#include "logger.h"
#include "localconfig.h"
#include "globalconfig.h"

#include <QTextStream>
#include <QDateTime>
#include <QSysInfo>

QString Logger::fileName;
unsigned int Logger::counter;
LocalConfig::LogfileMode Logger::logfileMode;
bool Logger::active;

static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(nullptr);

Logger::Logger(QObject *parent) : QObject(parent) {
    logfileMode = LocalConfig::logfileMode();
    if(logfileMode == LocalConfig::NoLog)
        return;

    fileName = "logfile.txt";
    counter = 0;

    QFile f(fileName);
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream s(&f);

    QDateTime now = QDateTime::currentDateTime();

    QString logfileModeInfo;
    if(logfileMode == LocalConfig::DefaultLog)
        logfileModeInfo = "Normal logging";
    if(logfileMode == LocalConfig::DebugLog)
        logfileModeInfo = "Debug logging";
    if(logfileMode == LocalConfig::DebugDetailLog)
        logfileModeInfo = "Detailed debug logging";

    s << "##########################################################################################\n";
    s << "   ScheduleMaster | " << GlobalConfig::currentVersion() << " | " << "Qt " << qVersion() << " | " << logfileModeInfo << "\n";
    s << "##########################################################################################\n";
    s << "Time:               " << now.toString("yyyy-MM-dd") << ", " << now.toString("hh:mm:ss") << "\n";
    s << "------------------------------------------------------------------------------------------\n";
    s << "Build information:  " << QSysInfo::buildAbi() << "\n";
    s << "------------------------------------------------------------------------------------------\n";
    s << "System Information: OS:               " << QSysInfo::prettyProductName() << "\n";
    s << "                    Kernel:           " << QSysInfo::kernelType() << " (" << QSysInfo::kernelVersion() << ")\n";
    s << "                    CPU architecture: " << QSysInfo::currentCpuArchitecture() << "\n";
    s << "------------------------------------------------------------------------------------------\n";
    s << "No.    | Time     | Type                    | Message\n";

    f.close();

    qInstallMessageHandler(Logger::handler);
}


void Logger::handler(QtMsgType type, const QMessageLogContext &context, const QString &message) {
    if(type == QtMsgType::QtDebugMsg && logfileMode != LocalConfig::DebugLog && logfileMode != LocalConfig::DebugDetailLog)
        return;

    QFile f(fileName);
    f.open(QIODevice::Append | QIODevice::Text);
    QTextStream s(&f);


    QString typeStr;
    if(type == QtMsgType::QtDebugMsg)
        typeStr = "            [debug]    ";
    else if(type == QtMsgType::QtInfoMsg)
        typeStr = "         [info]        ";
    else if(type == QtMsgType::QtWarningMsg)
        typeStr = "      [warning]        ";
    else if(type == QtMsgType::QtCriticalMsg)
        typeStr = "   [critical]          ";
    else if(type == QtMsgType::QtFatalMsg)
        typeStr = "[fatal]                ";
    else if(type == QtMsgType::QtSystemMsg)
        typeStr = "               [system]";
    else
        typeStr = "[unknown]              ";

    QString counterStr = QVariant(counter).toString() + ".";
    while(counterStr.length() < 6)
        counterStr += " ";

    QTime now = QTime::currentTime();
    QString timeStr = now.toString("hh:mm:ss");

    if(logfileMode == LocalConfig::DebugDetailLog) {
        s << "       |          |                         | " << message << "\n";

        s << counterStr << " | " << timeStr << " | " << typeStr << " | ";
        s << "in " << context.file << " (Line " << context.line << ")\n";

        s << "       |          |                         | " << context.function << "\n\n";
    } else {
        s << counterStr << " | " << timeStr << " | " << typeStr << " | " << message << "\n";
    }

    f.close();

    (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, message);

    counter++;
}
















