#ifndef SCHEDULEMASTER_H
#define SCHEDULEMASTER_H

#include "ScheduleMaster_global.h"

#include "ICrashDetector.h"
#include "ILogger.h"

namespace ScheduleMaster {

class SCHEDULEMASTERINTERFACE_EXPORT IApplicationInterface {
public:
    virtual ~IApplicationInterface() = default;

    virtual ICrashDetector *crashDetector() const = 0;
    virtual ILogger *logger() const = 0;
};

}

#endif // SCHEDULEMASTER_H
