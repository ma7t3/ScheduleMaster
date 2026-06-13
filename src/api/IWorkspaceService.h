#ifndef IWORKSPACESERVICE_H
#define IWORKSPACESERVICE_H

#include "ScheduleMaster_global.h"
#include "helpers.h"
#include "GlobalConfigItem.h"

#include <QJsonArray>
#include <QKeySequence>

namespace ScheduleMaster {

struct WorkspaceDockConfig : public GlobalConfigItem {
    WorkspaceDockConfig(const QJsonObject &jsonObject);

    bool visible;
    Qt::DockWidgetArea area;
};

struct WorkspaceResizeConfig : public GlobalConfigItem {
    WorkspaceResizeConfig(const QJsonObject &jsonObject);

    QStringList dockIDs;
    QList<float> sizes;
    Qt::Orientation orientation;
};

struct WorkspaceSplitConfig : public GlobalConfigItem {
    WorkspaceSplitConfig(const QJsonObject &jsonObject);

    QString firstID, secondID;
    Qt::Orientation orientation;
};

struct WorkspaceTabifyConfig : public GlobalConfigItem {
    WorkspaceTabifyConfig(const QJsonObject &jsonObject);

    QString firstID, secondID;
};

struct WorkspaceLayout : public GlobalConfigItem {
    WorkspaceLayout(const QJsonObject &jsonObject = QJsonObject());

    QList<WorkspaceDockConfig>   dockConfigs;
    QList<WorkspaceResizeConfig> resizeConfigs;
    QList<WorkspaceSplitConfig>  splitConfigs;
    QList<WorkspaceTabifyConfig> tabifyConfigs;
    QMap<Qt::Corner, Qt::DockWidgetArea> corners;

protected:
    Qt::DockWidgetArea parseAreaString(const QString &string);
};


class WorkspaceConfig : public GlobalConfigItem {
public:
    WorkspaceConfig(const QString &id, const int &index = 0);
    WorkspaceConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);

    QString name, icon;
    WorkspaceLayout layout;
    QKeySequence defaultKeyboardShortcut;
};

class IWorkspaceService {
public:
    virtual ~IWorkspaceService() = default;

    enum SpecialWorkspaceRole {
        OnApplicationStartupWorkspace,
        OnProjectOpenWorkspace,
        OnProjectCloseWorkspace
    };

    virtual QList<WorkspaceConfig> workspaces() const = 0;
    virtual QMap<QString, WorkspaceConfig> workspacesMap() const = 0;
    virtual QStringList workspaceIDs() const = 0;
    virtual WorkspaceConfig workspace(const QString &id) const = 0;
    virtual bool workspaceExists(const QString &id) const = 0;
    virtual bool registerWorkspace(const WorkspaceConfig &workspace) = 0;

    virtual QString currentWorkspaceID() const = 0;
    virtual QString workspaceForSpecialRole(SpecialWorkspaceRole role) const = 0;

    virtual void setCurrentWorkspace(const QString &workspaceID) = 0;
    virtual void restoreWorkspaceLayout(const QString &workspaceID) = 0;
    virtual void restoreCurrentWorkspaceLayout() = 0;
};

} // namespace ScheduleMaster

#endif // IWORKSPACESERVICE_H
