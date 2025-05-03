#include "WorkspaceManager.h"

WorkspaceManager::WorkspaceManager(QObject *parent) :
    GlobalConfigManager(parent) {
    loadItems("Workspaces");
}
