#include "DockManager.h"

#include "Global/Global.h"
#include "namespace.h"
#include "src/core/ActionServiceImpl.h"

DockConfig::DockConfig(const QJsonObject &jsonObject, const int &index) : GlobalConfigItem(jsonObject, index) {
    name = jsonObject.value("name").toString();
    icon = jsonObject.value("icon").toString();
    defaultKeyboardShortcut = Global::parseKeyboardShortcutConfigString(jsonObject.value("defaultKeyboardShortcut"));
}

void DockManager::init() {
    GlobalConfigManager::init();

    loadItems("Docks");
    const QList<DockConfig> itemList = items();
    for(const DockConfig &dock : itemList) {
        SMA::ActionConfig action(QString("view.docks.%1.toggle").arg(dock.id()));
        action.text                    = dock.name;
        action.description             = tr("Show/hide dock: %1").arg(dock.name);
        action.icon                    = dock.icon;
        action.breadcrumb              = {tr("View"), tr("Workspaces")};
        action.canHaveShortcut         = true;
        action.defaultKeyboardShortcut = dock.defaultKeyboardShortcut;
        SM::ActionServiceImpl::instance()->registerAction(action);
    }
}
