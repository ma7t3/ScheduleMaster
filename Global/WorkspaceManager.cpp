#include "WorkspaceManager.h"

#include "ActionManager.h"

WorkspaceManager::WorkspaceManager(QObject *parent) :
    GlobalConfigManager(parent) {
    loadItems("Workspaces");

    for(WorkspaceConfig &workspace : items()) {
        ActionConfig action(QString("view.workspaces.%1.activate").arg(workspace.id()));
        action.text                    = workspace.name;
        action.description             = tr("Switch to workspace: %1").arg(workspace.name);
        action.icon                    = workspace.icon;
        action.breadcrumb              = {tr("View"), tr("Workspaces")};
        action.canHaveShortcut         = true;
        action.defaultKeyboardShortcut = workspace.defaultKeyboardShortcut;
        ActionManager::addItem(action);
    }
}
