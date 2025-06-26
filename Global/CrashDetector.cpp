#include "CrashDetector.h"

#include "SettingsManager.h"

CrashDetector::~CrashDetector() {
    SettingsManager::setValue("general.closeCheck", true);
}

void CrashDetector::init() {
    if(initialized())
        return;

    const QVariant rawValue = SettingsManager::value("general.closeCheck");
    _lastCrashDetected = rawValue.isValid() && !rawValue.toBool();
    SettingsManager::setValue("general.closeCheck", false);

    // trigger instance once to ensure it is created and the destructor is called later
    instance();
    Singleton::init();
}

bool CrashDetector::crashDetected() {
    return _lastCrashDetected;
}
