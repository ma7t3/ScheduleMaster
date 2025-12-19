#include "ApplicationInterfaceImpl.h"

namespace ScheduleMaster::Core {

ApplicationInterfaceImpl::ApplicationInterfaceImpl(QObject *parent) : QObject(parent) {
    _self = this;
}

ApplicationInterfaceImpl *ApplicationInterfaceImpl::instance() {
    return _self;
}

}
