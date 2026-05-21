#include "IActionService.h"

#include "helpers.h"

namespace ScheduleMaster {

ScheduleMaster::ActionConfig::ActionConfig(const QJsonObject &jsonObject, const int &index) : GlobalConfigItem(jsonObject, index) {
    text        = jsonObject.value("text").toString(id());
    tooltip     = jsonObject.value("tooltip").toString();
    description = jsonObject.value("description").toString(text);
    icon        = jsonObject.value("icon").toString();
    breadcrumb  = jsonObject.value("breadcrumb").toString().split(" > ");

    if(breadcrumb.join("").isEmpty())
        breadcrumb.clear();

    canHaveShortcut         = jsonObject.contains("defaultKeyboardShortcut");
    defaultKeyboardShortcut = parseKeyboardShortcutConfigString(
        jsonObject.value("defaultKeyboardShortcut"));
}

ScheduleMaster::ActionConfig::ActionConfig(const QString &id, const int &index) :
    GlobalConfigItem(id, index) {}
}