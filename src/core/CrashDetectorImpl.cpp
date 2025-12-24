#include "CrashDetectorImpl.h"

#include "src/namespace.h"
#include "src/core/SettingsServiceImpl.h"

namespace ScheduleMaster::Core {

CrashDetectorImpl::CrashDetectorImpl(QObject *parent) : QObject(parent) {
    const QVariant rawValue = SM::SettingsServiceImpl::instance()->value("general.closeCheck");
    _crashDetected = rawValue.isValid() && !rawValue.toBool();
    SM::SettingsServiceImpl::instance()->setValue("general.closeCheck", false);
}

void CrashDetectorImpl::shutdown() {
    SM::SettingsServiceImpl::instance()->setValue("general.closeCheck", true);
}

bool CrashDetectorImpl::crashDetected() const {
    return _crashDetected;
}

}
