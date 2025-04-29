#include "WorkspaceHandler.h"

WorkspaceHandler::WorkspaceHandler(QObject *parent) : QObject(parent), _workspacesMenu(nullptr), _workspacesToolbar(nullptr) {
    setupStandardWorkspaces();
}

QList<Workspace *> WorkspaceHandler::workspaces() {
    return _workspaces;
}

Workspace *WorkspaceHandler::workspace(StandardWorkspaces workspace) {
    int workspaceIndex = static_cast<int>(workspace);
    if(workspaceIndex <= _standardWorkspaces.count())
        return _standardWorkspaces[workspaceIndex];
    else
        return nullptr;
}

void WorkspaceHandler::addWorkspace(Workspace *workspace) {
    if(!workspace)
        return;

    _workspaces.append(workspace);
    workspace->setParent(this);
    connect(workspace, &Workspace::activated, this, &WorkspaceHandler::onWorkspaceActivated);
    updateWorkspacesMenu();
    updateWorkspacesToolbar();
}

void WorkspaceHandler::addWorkspaces(QList<Workspace *> workspaces) {
    for(Workspace *workspace : workspaces)
        addWorkspace(workspace);
}

void WorkspaceHandler::removeWorkspace(Workspace *workspace) {
    if(_standardWorkspaces.contains(workspace))
        return;

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

void WorkspaceHandler::setupStandardWorkspaces() {
    QList<Workspace *> list;

    Workspace *homeWorkspace       = new Workspace(tr("Home"),       QIcon(":/Icons/Home.ico"),           this);
    Workspace *routingWorkspace    = new Workspace(tr("Routing"),    QIcon(":/Icons/Route.ico"),          this);
    Workspace *schedulingWorkspace = new Workspace(tr("Scheduling"), QIcon(":/Icons/Schedule.ico"),       this);
    Workspace *toursWorkspace      = new Workspace(tr("Tours"),      QIcon(":/Icons/Tour.ico"),           this);
    Workspace *publishWorkspace    = new Workspace(tr("Publish"),    QIcon(":/Icons/PublishedLines.ico"), this);

    list << homeWorkspace << routingWorkspace << schedulingWorkspace << toursWorkspace << publishWorkspace;

    addWorkspaces(list);
    _standardWorkspaces = list;
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
}
