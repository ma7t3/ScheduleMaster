#include "globalconfig.h"

const QString GlobalConfig::_currentVersion = "0.9.0-beta";

GlobalConfig::GlobalConfig(QObject *parent) : QObject(parent) {}

const QString &GlobalConfig::currentVersion() {
    return _currentVersion;
}
