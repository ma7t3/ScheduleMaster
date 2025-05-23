#include "DockManager.h"

DockConfig::DockConfig(const QJsonObject &jsonObject, const int &index) : GlobalConfigItem(jsonObject, index) {
    name = jsonObject.value("name").toString();
    icon = jsonObject.value("icon").toString();
}

DockConfig::DockConfig(const QString &id, const int &index) : GlobalConfigItem(id, index) {}

DockManager::DockManager(QObject *parent) : GlobalConfigManager(parent) {
    loadItems("Docks");
}
