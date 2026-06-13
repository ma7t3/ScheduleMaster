#include "ScheduleMaster.h"

namespace ScheduleMaster {

IApplicationInterface* IApplicationInterface::_self = nullptr;

IApplicationInterface::IApplicationInterface() {
    _self = this;
}

IApplicationInterface* IApplicationInterface::instance() {
    return _self;
}

}