#include "IconSetManager.h"

#include "Global/SettingsManager.h"

IconSetConfig::IconSetConfig(const QJsonObject &jsonObject) : GlobalConfigItem(jsonObject) {
    name = jsonObject.value("name").toString();
    alternative = jsonObject.value("alternative").toString();
    format = jsonObject.value("format").toString();
}

IconSetConfig::IconSetConfig(const QString &id) : GlobalConfigItem(id) {}

IconSetManager::IconSetManager(QObject *parent) : GlobalConfigManager(parent) {
    loadItems("IconSets");
}

QString IconSetManager::currentIconSet() {
    return SettingsManager::value("appearance.iconSet").toString();
}

void IconSetManager::setCurrentIconSet(const QString &iconSetID) {
    SettingsManager::setValue("appearance.iconSet", iconSetID);
    emit instance()->currentIconSetChanged(iconSetID);
}
