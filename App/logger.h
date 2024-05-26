#ifndef LOGGER_H
#define LOGGER_H

#include "localconfig.h"

#include <QObject>

#include <QFile>

class Logger : public QObject {
    Q_OBJECT
public:
    explicit Logger(QObject *parent = nullptr, const QDir &logfilesDir = QDir());
    static void handler(QtMsgType type, const QMessageLogContext & context, const QString &message);

private:
    static inline QString logfilePath;
    static inline unsigned int counter;
    static inline LocalConfig::LogfileMode logfileMode;
    static inline bool active;
};

#endif // LOGGER_H
