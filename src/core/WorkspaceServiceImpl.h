#ifndef WORKSPACESERVICEIMPL_H
#define WORKSPACESERVICEIMPL_H

#include "IWorkspaceService.h"
#include "GlobalConfigService.h"
#include "GlobalConfigRepository.h"

#include <QPointer>

class QMainWindow;
class QMenu;
class QToolBar;
class QAction;

namespace ScheduleMaster::Core {

class WorkspaceHandle;

using WorkspaceRepository = GlobalConfigRepositoryCRTP<WorkspaceConfig>;

class WorkspaceServiceImpl :
    public GlobalConfigServiceCRTP<WorkspaceRepository, WorkspaceServiceImpl>,
    public IWorkspaceService {
public:
    WorkspaceServiceImpl(QMainWindow *mainWindow, QObject *parent);

    virtual QList<WorkspaceConfig> workspaces() const override;
    virtual QMap<QString, WorkspaceConfig> workspacesMap() const override;
    virtual QStringList workspaceIDs() const override;
    virtual WorkspaceConfig workspace(const QString &id) const override;
    virtual bool workspaceExists(const QString &id) const override;
    virtual bool registerWorkspace(const WorkspaceConfig &workspace) override;

    virtual QString currentWorkspaceID() const override;
    virtual QString workspaceForSpecialRole(SpecialWorkspaceRole role) const override;
    virtual void setCurrentWorkspace(const QString &workspaceID) override;
    virtual void restoreWorkspaceLayout(const QString &workspaceID) override;
    virtual void restoreCurrentWorkspaceLayout() override;

    void setCurrentSpecialRoleWorkspace(SpecialWorkspaceRole role);

    void setWorkspacesMenu(QMenu *newMenu);
    void setWorkspacesToolbar(QToolBar *newToolBar);

protected:
    void updateWorkspacesMenu();
    void updateWorkspacesToolbar();

    void createWorkspaceHandle(const WorkspaceConfig &workspace);
    void setCurrentWorkspace(WorkspaceHandle *handle);

    void updateSortedHandles();

protected slots:
    void onWorkspaceActionTriggered(ScheduleMaster::Core::WorkspaceHandle *handle, bool checked);

private:
    QMainWindow *_mainWindow;
    QMap<QString, WorkspaceHandle *> _handles;
    QList<WorkspaceHandle *> _sortedHandles;
    WorkspaceHandle *_currentWorkspaceHandle;
    QPointer<QMenu> _workspacesMenu;
    QPointer<QToolBar> _workspacesToolbar;
    QAction *_restoreLayoutAction;
};

} // namespace ScheduleMaster::Core

#endif // WORKSPACESERVICEIMPL_H
