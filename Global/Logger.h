#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QDir>

#include "LocalConfig.h"

class Logger : public QObject {
    Q_OBJECT
public:
    explicit Logger(QObject *parent, const QDir &logfilesDirectory);
    static void handler(QtMsgType type, const QMessageLogContext & context, const QString &message);

    static void setPrefix(const QString &prefix);
    static void resetPrefix();

    static void beginSub();
    static void endSub();

private:
    static inline QString _logfilePath;
    static inline unsigned int _counter;
    static inline LocalConfig::LogfileMode _logfileMode;

    static inline QString _prefix;
    static inline int _subCount;
};

#endif // LOGGER_H
