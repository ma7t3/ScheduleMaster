#ifndef SCHEDULEMASTER_H
#define SCHEDULEMASTER_H

#include "ScheduleMaster_global.h"

#include "ICrashDetector.h"

namespace ScheduleMaster {

class SCHEDULEMASTERINTERFACE_EXPORT IApplicationInterface {
public:
    virtual ~IApplicationInterface() = default;

    virtual ICrashDetector *crashDetector() const = 0;
};

}

#endif // SCHEDULEMASTER_H
