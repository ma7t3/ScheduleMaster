#include "DockManager.h"

#include "Global/Global.h"
#include "Global/ActionManager.h"

DockConfig::DockConfig(const QJsonObject &jsonObject, const int &index) : GlobalConfigItem(jsonObject, index) {
    name = jsonObject.value("name").toString();
    icon = jsonObject.value("icon").toString();
    defaultKeyboardShortcut = Global::parseKeyboardShortcutConfigString(jsonObject.value("defaultKeyboardShortcut"));
}

DockConfig::DockConfig(const QString &id, const int &index) : GlobalConfigItem(id, index) {}

DockManager::DockManager(QObject *parent) : GlobalConfigManager(parent) {
    loadItems("Docks");

    for(DockConfig &dock : items()) {
        ActionConfig action(QString("view.docks.%1.toggle").arg(dock.id()));
        action.text                    = dock.name;
        action.description             = tr("Show/hide dock: %1").arg(dock.name);
        action.icon                    = dock.icon;
        action.breadcrumb              = {tr("View"), tr("Workspaces")};
        action.canHaveShortcut         = true;
        action.defaultKeyboardShortcut = dock.defaultKeyboardShortcut;
        ActionManager::addItem(action);
    }
}
