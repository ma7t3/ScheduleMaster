#include "IDockService.h"

#include "helpers.h"

namespace ScheduleMaster {

DockConfig::DockConfig(const QString &id, const int &index) : GlobalConfigItem(id, index) {
}

DockConfig::DockConfig(const QJsonObject &jsonObject, const int &index) : GlobalConfigItem(jsonObject, index) {
    name = jsonObject.value("name").toString();
    icon = jsonObject.value("icon").toString();
    defaultKeyboardShortcut = parseKeyboardShortcutConfigString(jsonObject.value("defaultKeyboardShortcut"));
}

}