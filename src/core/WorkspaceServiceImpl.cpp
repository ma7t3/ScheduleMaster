#include "WorkspaceServiceImpl.h"

#include "core/SettingsServiceImpl.h"
#include "core/ActionServiceImpl.h"
#include "core/WorkspaceHandle.h"

#include <QDockWidget>
#include <QMenu>
#include <QToolBar>

namespace ScheduleMaster::Core {

WorkspaceServiceImpl::WorkspaceServiceImpl(QMainWindow *mainWindow, QObject *parent) :
    GlobalConfigServiceCRTP(parent, "Workspaces"), _mainWindow{mainWindow},
    _currentWorkspaceHandle{nullptr} {
    _restoreLayoutAction = new QAction(this);
    ActionServiceImpl::instance()->addAction(_restoreLayoutAction, "view.workspaces.restoreDefaultLayout");
    ActionServiceImpl::instance()->setGlobalAction("view.workspaces.restoreDefaultLayout", _restoreLayoutAction);
    connect(_restoreLayoutAction, &QAction::triggered, this, &WorkspaceServiceImpl::restoreCurrentWorkspaceLayout);

    initRepository();

    for(WorkspaceConfig &workspace : repository()->sortedItems()) {
        ActionConfig action(QString("view.workspaces.%1.activate").arg(workspace.id()));
        action.text                    = workspace.name;
        action.description             = tr("Switch to workspace: %1").arg(workspace.name);
        action.icon                    = workspace.icon;
        action.breadcrumb              = {tr("View"), tr("Workspaces")};
        action.canHaveShortcut         = true;
        action.defaultKeyboardShortcut = workspace.defaultKeyboardShortcut;
        ActionServiceImpl::instance()->registerAction(action);
        createWorkspaceHandle(workspace);
    }
}

QList<WorkspaceConfig> WorkspaceServiceImpl::workspaces() const {
    return repository()->items();
}

QMap<QString, WorkspaceConfig> WorkspaceServiceImpl::workspacesMap() const {
    return repository()->itemsMap();
}

QStringList WorkspaceServiceImpl::workspaceIDs() const {
    return repository()->itemIDs();
}

WorkspaceConfig WorkspaceServiceImpl::workspace(const QString &id) const {
    return repository()->item(id);
}

bool WorkspaceServiceImpl::workspaceExists(const QString &id) const {
    return repository()->itemExists(id);
}

bool WorkspaceServiceImpl::registerWorkspace(const WorkspaceConfig &workspace) {
    const bool result = repository()->addItem(workspace);
    if(result)
        createWorkspaceHandle(workspace);

    return result;
}

QString WorkspaceServiceImpl::currentWorkspaceID() const {
    return _currentWorkspaceHandle ? _currentWorkspaceHandle->id() : "";
}

QString WorkspaceServiceImpl::workspaceForSpecialRole(SpecialWorkspaceRole role) const {
    const QString settingID = role == OnApplicationStartupWorkspace
                                  ? "workspaces.onApplicationStartupWorkspace"
                              : role == OnProjectOpenWorkspace ? "workspaces.onProjectOpenWorkspace"
                              : role == OnProjectCloseWorkspace
                                  ? "workspaces.onProjectCloseWorkspace"
                                  : "";

    return SettingsServiceImpl::instance()->value(settingID).toString();
}

void WorkspaceServiceImpl::setCurrentWorkspace(const QString &workspaceID) {
    auto handle = _handles.value(workspaceID, nullptr);
    if(handle)
        setCurrentWorkspace(handle);
}

void WorkspaceServiceImpl::setCurrentWorkspace(WorkspaceHandle *handle) {
    if(!handle)
        return;

    if(_currentWorkspaceHandle)
        _currentWorkspaceHandle->setActive(false);

    handle->apply();
    handle->setActive(true);
    _currentWorkspaceHandle = handle;
}

void WorkspaceServiceImpl::updateSortedHandles() {
    _sortedHandles.clear();
    _sortedHandles = _handles.values();
    std::sort(_sortedHandles.begin(),
              _sortedHandles.end(),
              [](WorkspaceHandle *a, WorkspaceHandle *b) { return a->index() < b->index(); });
}

void WorkspaceServiceImpl::setCurrentSpecialRoleWorkspace(SpecialWorkspaceRole role) {
    setCurrentWorkspace(workspaceForSpecialRole(role));
}

void WorkspaceServiceImpl::restoreWorkspaceLayout(const QString &workspaceID) {
    auto handle = _handles.value(workspaceID, nullptr);
    if(handle)
        handle->clearWindowState();
}

void WorkspaceServiceImpl::restoreCurrentWorkspaceLayout() {
    if(_currentWorkspaceHandle) {
        _currentWorkspaceHandle->clearWindowState();
        _currentWorkspaceHandle->apply();
    }
}

void WorkspaceServiceImpl::setWorkspacesMenu(QMenu *newMenu) {
    _workspacesMenu = newMenu;
    updateWorkspacesMenu();
}

void WorkspaceServiceImpl::setWorkspacesToolbar(QToolBar *newToolBar) {
    _workspacesToolbar = newToolBar;
    updateWorkspacesToolbar();
}

void WorkspaceServiceImpl::updateWorkspacesMenu() {
    if(!_workspacesMenu)
        return;

    _workspacesMenu->clear();
    for(WorkspaceHandle *handle : std::as_const(_sortedHandles))
        _workspacesMenu->addAction(handle->action());

    _workspacesMenu->addSeparator();
    _workspacesMenu->addAction(_restoreLayoutAction);
}

void WorkspaceServiceImpl::updateWorkspacesToolbar() {
    if(!_workspacesToolbar)
        return;

    _workspacesToolbar->clear();
    for(WorkspaceHandle *handle : std::as_const(_sortedHandles))
        _workspacesToolbar->addAction(handle->action());
}

void WorkspaceServiceImpl::createWorkspaceHandle(const WorkspaceConfig &workspace) {
    WorkspaceHandle *handle = new WorkspaceHandle(workspace, _mainWindow, this);
    _handles.insert(workspace.id(), handle);
    updateSortedHandles();
    connect(handle->action(), &QAction::triggered, this, [handle, this](bool checked) {
        onWorkspaceActionTriggered(handle, checked);
    });
    updateWorkspacesMenu();
    updateWorkspacesToolbar();
}

void WorkspaceServiceImpl::onWorkspaceActionTriggered(WorkspaceHandle *handle, bool checked) {
    if(checked)
        setCurrentWorkspace(handle);
    else {
        handle->setActive(true);
        restoreCurrentWorkspaceLayout();
    }
}

}