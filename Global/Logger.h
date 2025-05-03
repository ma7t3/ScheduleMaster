#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QDir>

class Logger : public QObject {
    Q_OBJECT
public:
    enum LogfileMode {
        NoLog,
        DefaultLog,
        DebugLog,
        DebugDetailLog
    };

    explicit Logger(QObject *parent);
    static void handler(QtMsgType type, const QMessageLogContext & context, const QString &message);

    static void setPrefix(const QString &prefix);
    static void resetPrefix();

    static void beginSub();
    static void endSub();

    static LogfileMode logfileMode();
    static void setLogfileMode(const LogfileMode &newLogfileMode);

    static bool currentLogfileExists();
    static QString lastLogfilePath();

private:
    static inline QString _logfilePath;
    static inline QString _lastLogfilePath;
    static inline unsigned int _counter;
    static inline LogfileMode _logfileMode;

    static inline QString _prefix;
    static inline int _subCount;
};

#endif // LOGGER_H
