#ifndef LOGGERIMPL_H
#define LOGGERIMPL_H

#include <QObject>

#include "Service.h"
#include "src/api/ILogger.h"

namespace ScheduleMaster::Core {

class LoggerImpl : public QObject, public Service<LoggerImpl>, public ILogger {
    Q_OBJECT
public:
    explicit LoggerImpl(QObject *parent = nullptr);

    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message);

    virtual void log(const QtMsgType &type, const QMessageLogContext &context, const QString &message) override;

    virtual LogfileMode logfileMode() const override;
    virtual void setLogfileMode(const LogfileMode &newLogfileMode) override;

    virtual bool currentLogfileExists() const override;
    virtual QString lastLogfilePath() const override;

signals:

private:
    static inline LoggerImpl *_instance = nullptr;
    QString _logfilePath;
    QString _lastLogfilePath;
    unsigned int _counter;
    LogfileMode _logfileMode;
    QString _prefix;
};

}

#endif // LOGGERIMPL_H
