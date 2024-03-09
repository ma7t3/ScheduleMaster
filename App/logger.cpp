#include "logger.h"

#include <QTextStream>

#include <QDateTime>

QString Logger::fileName;
unsigned int Logger::counter;

Logger::Logger(QObject *parent) : QObject(parent) {
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

    s << counterStr << " | " << timeStr << " | " << typeStr << " | " << message << "\n";
    f.close();
    counter++;
}
















