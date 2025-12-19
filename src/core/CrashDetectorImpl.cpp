#include "CrashDetectorImpl.h"

#include "Global/SettingsManager.h"

namespace ScheduleMaster::Core {

CrashDetectorImpl::CrashDetectorImpl(QObject *parent) : QObject(parent) {
    const QVariant rawValue = SettingsManager::value("general.closeCheck");
    _crashDetected = rawValue.isValid() && !rawValue.toBool();
    SettingsManager::setValue("general.closeCheck", false);
}

CrashDetectorImpl::~CrashDetectorImpl() {
    SettingsManager::setValue("general.closeCheck", true);
}

bool CrashDetectorImpl::crashDetected() const {
    return _crashDetected;
}

}
