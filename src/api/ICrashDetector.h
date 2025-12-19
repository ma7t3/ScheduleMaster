#ifndef ICRASHDETECTOR_H
#define ICRASHDETECTOR_H

#include "ScheduleMaster_global.h"

namespace ScheduleMaster {

class SCHEDULEMASTERINTERFACE_EXPORT ICrashDetector {
public:
    virtual ~ICrashDetector() = default;

    virtual bool crashDetected() const = 0;
};

}

#endif // ICRASHDETECTOR_H
