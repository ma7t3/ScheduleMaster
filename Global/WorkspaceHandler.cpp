#include "WorkspaceHandler.h"

WorkspaceHandler::WorkspaceHandler(QObject *parent) : QObject(parent), _workspacesMenu(nullptr), _workspacesToolbar(nullptr) {
    loadWorkspaces();
}

QList<Workspace *> WorkspaceHandler::workspaces() {
    return _workspaces;
}

Workspace *WorkspaceHandler::workspace(const QString &id) {
    for(Workspace *w : std::as_const(_workspaces))
        if(w->id() == id)
            return w;

    return nullptr;
}

void WorkspaceHandler::addWorkspace(Workspace *workspace) {
    if(!workspace)
        return;

    qDebug() << workspace->action();

    _workspaces << workspace;
    connect(workspace, &Workspace::activated, this, &WorkspaceHandler::onWorkspaceActivated);
    updateWorkspacesMenu();
    updateWorkspacesToolbar();
}

void WorkspaceHandler::addWorkspaces(QList<Workspace *> workspaces) {
    for(Workspace *workspace : workspaces)
        addWorkspace(workspace);
}

void WorkspaceHandler::removeWorkspace(Workspace *workspace) {
    _workspaces.removeAll(workspace);
    workspace->disconnect(this);
    updateWorkspacesMenu();
    updateWorkspacesToolbar();
}

void WorkspaceHandler::setWorkspacesMenu(QMenu *newMenu) {
    _workspacesMenu = newMenu;
    updateWorkspacesMenu();
}

void WorkspaceHandler::setWorkspacesToolbar(QToolBar *newToolBar) {
    _workspacesToolbar = newToolBar;
    updateWorkspacesToolbar();
}

Workspace *WorkspaceHandler::currentWorkspace() {
    for(Workspace *workspace : std::as_const(_workspaces))
        if(workspace->active())
            return workspace;

    return nullptr;
}

void WorkspaceHandler::loadWorkspaces() {
    qInfo() << "   Loading workspaces...";
    const QJsonArray workspaces = GlobalConfig::loadMultiConfigResource("Workspaces");
    for(const QJsonValue &val : workspaces) {
        const QJsonObject obj = val.toObject();
        Workspace *workspace = new Workspace(obj, this);
        if(workspace->id().isEmpty()) {
            delete workspace;
            continue;
        }

        addWorkspace(workspace);
        qInfo().noquote() << "      - " + workspace->id();
    }
}

void WorkspaceHandler::updateWorkspacesMenu() {
    if(!_workspacesMenu)
        return;

    _workspacesMenu->clear();
    for(Workspace *workspace : std::as_const(_workspaces))
        _workspacesMenu->addAction(workspace->action());
}

void WorkspaceHandler::updateWorkspacesToolbar() {
    if(!_workspacesToolbar)
        return;

    _workspacesToolbar->clear();
    for(Workspace *workspace : std::as_const(_workspaces))
        _workspacesToolbar->addAction(workspace->action());
}

void WorkspaceHandler::onWorkspaceActivated(Workspace *workspace) {
    for(Workspace *current : std::as_const(_workspaces))
        if(current != workspace)
            current->deactivate();

    workspace->apply();
}
