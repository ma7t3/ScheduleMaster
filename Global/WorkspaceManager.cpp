#include "WorkspaceManager.h"

#include "namespace.h"
#include "src/core/ActionServiceImpl.h"

WorkspaceManager::WorkspaceManager(QObject *parent) :
    GlobalConfigManager(parent) {
    loadItems("Workspaces");

    for(WorkspaceConfig &workspace : items()) {
        SMA::ActionConfig action(QString("view.workspaces.%1.activate").arg(workspace.id()));
        action.text                    = workspace.name;
        action.description             = tr("Switch to workspace: %1").arg(workspace.name);
        action.icon                    = workspace.icon;
        action.breadcrumb              = {tr("View"), tr("Workspaces")};
        action.canHaveShortcut         = true;
        action.defaultKeyboardShortcut = workspace.defaultKeyboardShortcut;
        SM::ActionServiceImpl::instance()->registerAction(action);
    }
}
