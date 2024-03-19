#include "logger.h"
#include "localconfig.h"
#include "globalconfig.h"

#include <QTextStream>

#include <QDateTime>

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

    s << "ScheduleMaster\n";
    s << "Version: 0.9.0-Beta\n";
    s << "Date:    " << now.toString("yyyy-MM-dd") << "\n";
    s << "Time:    " << now.toString("hh:mm:ss") << "\n\n";
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
















