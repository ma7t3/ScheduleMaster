#ifndef LOGGER_H
#define LOGGER_H

#include "localconfig.h"

#include <QObject>

#include <QFile>

class Logger : public QObject {
    Q_OBJECT
public:
    explicit Logger(QObject *parent = nullptr);
    static void handler(QtMsgType type, const QMessageLogContext & context, const QString &message);

private:
    static QString fileName;
    static unsigned int counter;
    static LocalConfig::LogfileMode logfileMode;
    static bool active;
};

#endif // LOGGER_H
