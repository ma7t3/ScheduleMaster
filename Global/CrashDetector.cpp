#include "CrashDetector.h"

#include "SettingsManager.h"

CrashDetector::~CrashDetector() {
    SettingsManager::setValue("general.closeCheck", true);
}

void CrashDetector::init() {
    _lastCrashDetected = !SettingsManager::value("general.closeCheck").toBool();
    SettingsManager::setValue("general.closeCheck", false);
    instance();
}

bool CrashDetector::crashDetected() {
    return _lastCrashDetected;
}
