#include "ISettingsService.h"

// TODO: [core-rework] remove/fix
#include "Global/VariantConverter.h"

namespace ScheduleMaster {

SettingsItem::SettingsItem(const QString &id, const int &index) : GlobalConfigItem(id, index) {}

SettingsItem::SettingsItem(const QJsonObject &jsonObject, const int &index) : GlobalConfigItem(jsonObject, index) {
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

    requiresRestart  = jsonObject.value("requireRestart").toBool(false);
    dontTouchWarning = jsonObject.value("dontTouchWarning").toBool(false);
}

}
