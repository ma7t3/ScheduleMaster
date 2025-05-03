#include "DockManager.h"

DockConfig::DockConfig(const QJsonObject &jsonObject) : GlobalConfigItem(jsonObject) {
    name = jsonObject.value("name").toString();
    icon = jsonObject.value("icon").toString();
}

DockConfig::DockConfig(const QString &id) : GlobalConfigItem(id) {}

DockManager::DockManager(QObject *parent) : GlobalConfigManager(parent) {
    loadItems("Docks");
}
