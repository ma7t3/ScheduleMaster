#include "IconSetManager.h"

#include "src/namespace.h"
#include "src/core/SettingsServiceImpl.h"

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
    return SM::SettingsServiceImpl::instance()->value("appearance.iconSet").toString();
}

void IconSetManager::setCurrentIconSet(const QString &iconSetID) {
    SM::SettingsServiceImpl::instance()->setValue("appearance.iconSet", iconSetID);
    emit instance()->currentIconSetChanged(iconSetID);
}
