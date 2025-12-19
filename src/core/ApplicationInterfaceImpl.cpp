#include "ApplicationInterfaceImpl.h"

#include "src/core/CrashDetectorImpl.h"
#include "src/core/LoggerImpl.h"

namespace ScheduleMaster::Core {

ApplicationInterfaceImpl::ApplicationInterfaceImpl(QObject *parent) : QObject(parent) {
    _self = this;

    _crashDetector = new CrashDetectorImpl(this);
    _logger = new LoggerImpl(this);
}

ApplicationInterfaceImpl *ApplicationInterfaceImpl::instance() {
    return _self;
}

ICrashDetector *ApplicationInterfaceImpl::crashDetector() const {
    return _crashDetector;
}

ILogger *ApplicationInterfaceImpl::logger() const {
    return _logger;
}

}
