#ifndef WORKSPACEMANAGER_H
#define WORKSPACEMANAGER_H

#include <QObject>
#include <QIcon>
#include "GlobalConfigManager.h"


class WorkspaceConfig : public GlobalConfigItem {
public:
    WorkspaceConfig(const QString &id) : GlobalConfigItem(id) {}
    WorkspaceConfig(const QJsonObject &jsonObject = QJsonObject()) : GlobalConfigItem(jsonObject) {
        name = jsonObject.value("name").toString();
        icon = QIcon(jsonObject.value("icon").toString());
    }

    QString name;
    QIcon icon;
};

class WorkspaceManager : public GlobalConfigManager<WorkspaceManager, WorkspaceConfig> {
    friend class GlobalConfigManager<WorkspaceManager, WorkspaceConfig>;
    Q_OBJECT
public:
    explicit WorkspaceManager(QObject *parent = nullptr);
};

#endif // WORKSPACEMANAGER_H
