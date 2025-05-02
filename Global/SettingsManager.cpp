#include "SettingsManager.h"

SettingsItem::SettingsItem(const QString &id) : GlobalConfigItem(id) {}

SettingsItem::SettingsItem(const QJsonObject &jsonObject) : GlobalConfigItem(jsonObject) {
    description = jsonObject.value("description").toString();

    QString typeStr = jsonObject.value("type").toString();
    if (typeStr == "group")
        isGroup = true;

    typeStr = isGroup ? jsonObject.value("content_type").toString() : typeStr;

    type = static_cast<QMetaType::Type>(QMetaType::fromName(typeStr.toUtf8()).id());

    if (isGroup) {
        groupContentType = type;
        type = QMetaType::Void;
    }

    if (!isGroup)
        if(jsonObject.contains("default"))
            defaultValue = VariantConverter::convertFromJson(jsonObject.value("default"), type);

    requiresRestart = jsonObject.value("requireRestart").toBool(false);
}

SettingsManager::SettingsManager(QObject *parent) :
    GlobalConfigManager(parent) {
    loadItems("Settings");
    qDebug() << "ITEM COUNTINGING";
    qDebug() << items().size();
    for(const SettingsItem &item : items())
        if(item.requiresRestart)
            _restartRequiredSettings << item.id();
}

QSet<QString> SettingsManager::restartRequiredSettings() {
    return _restartRequiredSettings;
}

bool SettingsManager::settingRequiresRestart(const QString &id) {
    return _restartRequiredSettings.contains(id);
}

void SettingsManager::registerNewSettingsItem(const SettingsItem &item) {
    if(item.id().isEmpty())
        return;

    if(item.requiresRestart)
        _restartRequiredSettings << item.id();

    addItem(item);
    qInfo().noquote() << "Registered new settings item: " + item.id();
}
