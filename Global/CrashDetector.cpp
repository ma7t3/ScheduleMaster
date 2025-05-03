#include "CrashDetector.h"

#include "SettingsManager.h"

CrashDetector::CrashDetector(QObject *parent) : QObject(parent) {
    _lastCrashDetected = !SettingsManager::value("general.closeCheck").toBool();
    SettingsManager::setValue("general.closeCheck", false);
}

CrashDetector::~CrashDetector() {
    SettingsManager::setValue("general.closeCheck", true);
}

CrashDetector *CrashDetector::instance() {
    static CrashDetector instance(nullptr);
    return &instance;
}

void CrashDetector::init() {
    instance();
}

bool CrashDetector::crashDetected() {
    return _lastCrashDetected;
}
