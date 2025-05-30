#ifndef WORKSPACEMANAGER_H
#define WORKSPACEMANAGER_H

#include <QObject>
#include <QIcon>
#include "GlobalConfigManager.h"

struct WorkspaceDockConfig : public GlobalConfigItem {
    WorkspaceDockConfig(const QJsonObject &jsonObject) : GlobalConfigItem(jsonObject) {
        visible  = jsonObject.value("visible").toBool(true);
        floating = jsonObject.value("floating").toBool(true);
        QString areaString = jsonObject.value("area").toString("left");
        area = areaString == "right"  ? Qt::RightDockWidgetArea :
               areaString == "top"    ? Qt::TopDockWidgetArea :
               areaString == "bottom" ? Qt::BottomDockWidgetArea :
               Qt::LeftDockWidgetArea;
    }

    bool visible, floating;
    Qt::DockWidgetArea area;
};

struct WorkspaceResizeConfig : public GlobalConfigItem {
    WorkspaceResizeConfig(const QJsonObject &jsonObject) : GlobalConfigItem(jsonObject) {
        dockIDs = jsonObject.value("docks").toVariant().toStringList();
        const QJsonArray sizesArray = jsonObject.value("sizes").toArray();
        for(const QJsonValue &val : sizesArray)
            sizes << val.toDouble();

        orientation = jsonObject.value("orientation").toString() == "horizontal" ? Qt::Horizontal : Qt::Vertical;
    }

    QStringList dockIDs;
    QList<float> sizes;
    Qt::Orientation orientation;
};

struct WorkspaceLayout : public GlobalConfigItem {
    WorkspaceLayout(const QJsonObject &jsonObject = QJsonObject()) : GlobalConfigItem(jsonObject) {
        QJsonArray docksArray = jsonObject.value("docks").toArray();
        for(const QJsonValue &dockVal : std::as_const(docksArray)) {
            QJsonObject dockObj = dockVal.toObject();
            WorkspaceDockConfig dockConfig(dockObj);
            dockConfigs << dockConfig;
        }

        QJsonArray resizeArray = jsonObject.value("resize").toArray();
        for(const QJsonValue &resizeVal : std::as_const(resizeArray)) {
            QJsonObject dockObj = resizeVal.toObject();
            WorkspaceResizeConfig resizeConfig(dockObj);
            resizeConfigs << resizeConfig;
        }
    }


    QList<WorkspaceDockConfig> dockConfigs;
    QList<WorkspaceResizeConfig> resizeConfigs;
};


class WorkspaceConfig : public GlobalConfigItem {
public:
    WorkspaceConfig(const QString &id, const int &index = 0) : GlobalConfigItem(id, index) {}
    WorkspaceConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0) :
        GlobalConfigItem(jsonObject, index),
        layout(WorkspaceLayout(jsonObject.value("layout").toObject())) {
        name = jsonObject.value("name").toString();
        icon = QIcon(jsonObject.value("icon").toString());
    }

    QString name;
    QIcon icon;
    WorkspaceLayout layout;
};

class WorkspaceManager : public GlobalConfigManager<WorkspaceManager, WorkspaceConfig> {
    friend class GlobalConfigManager<WorkspaceManager, WorkspaceConfig>;
    Q_OBJECT
public:
    explicit WorkspaceManager(QObject *parent = nullptr);
};

#endif // WORKSPACEMANAGER_H
