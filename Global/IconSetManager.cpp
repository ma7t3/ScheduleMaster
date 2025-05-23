#include "IconSetManager.h"

#include "Global/SettingsManager.h"

IconSetConfig::IconSetConfig(const QJsonObject &jsonObject, const int &index) : GlobalConfigItem(jsonObject, index) {
    name = jsonObject.value("name").toString();
    alternative = jsonObject.value("alternative").toString();
    format = jsonObject.value("format").toString();
}

IconSetConfig::IconSetConfig(const QString &id, const int &index) : GlobalConfigItem(id, index) {}

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
