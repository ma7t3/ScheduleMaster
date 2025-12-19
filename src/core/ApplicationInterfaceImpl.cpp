#include "ApplicationInterfaceImpl.h"

namespace ScheduleMaster::Core {

ApplicationInterfaceImpl::ApplicationInterfaceImpl(QObject *parent) : QObject(parent) {
    _crashDetector = new CrashDetectorImpl(this);

    _self = this;
}

ApplicationInterfaceImpl *ApplicationInterfaceImpl::instance() {
    return _self;
}

ICrashDetector *ApplicationInterfaceImpl::crashDetector() const {
    return _crashDetector;
}

}
