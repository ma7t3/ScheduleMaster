#ifndef ILOGGER_H
#define ILOGGER_H

#include "ScheduleMaster_global.h"

namespace ScheduleMaster {

class SCHEDULEMASTERINTERFACE_EXPORT ILogger {
public:
    virtual ~ILogger() = default;

    enum LogfileMode {
        NoLog,
        DefaultLog,
        DebugLog,
        DebugDetailLog
    };

    virtual void log(const QtMsgType &type, const QMessageLogContext &context, const QString &message) = 0;

    virtual LogfileMode logfileMode() const = 0;
    virtual void setLogfileMode(const LogfileMode &newLogfileMode) = 0;

    virtual bool currentLogfileExists() const = 0;
    virtual QString lastLogfilePath() const = 0;
};

}

#endif // ILOGGER_H
