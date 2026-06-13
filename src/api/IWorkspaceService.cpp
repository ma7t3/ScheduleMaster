#include "IWorkspaceService.h"

namespace ScheduleMaster {

WorkspaceDockConfig::WorkspaceDockConfig(const QJsonObject &jsonObject) :
    GlobalConfigItem(jsonObject) {
    visible = jsonObject.value("visible").toBool(true);
    QString areaString = jsonObject.value("area").toString("left");
    area = areaString == "right"    ? Qt::RightDockWidgetArea
           : areaString == "left"   ? Qt::LeftDockWidgetArea
           : areaString == "top"    ? Qt::TopDockWidgetArea
           : areaString == "bottom" ? Qt::BottomDockWidgetArea
                                    : Qt::NoDockWidgetArea;
}

WorkspaceResizeConfig::WorkspaceResizeConfig(const QJsonObject &jsonObject) :
    GlobalConfigItem(jsonObject) {
    dockIDs = jsonObject.value("docks").toVariant().toStringList();
    const QJsonArray sizesArray = jsonObject.value("sizes").toArray();
    for(const QJsonValue &val : sizesArray)
        sizes << val.toDouble();

    orientation = jsonObject.value("orientation").toString() == "horizontal" ? Qt::Horizontal
                                                                             : Qt::Vertical;
}

WorkspaceSplitConfig::WorkspaceSplitConfig(const QJsonObject &jsonObject) :
    GlobalConfigItem(jsonObject) {
    firstID = jsonObject.value("first").toString();
    secondID = jsonObject.value("second").toString();
    orientation = jsonObject.value("orientation").toString() == "horizontal" ? Qt::Horizontal
                                                                             : Qt::Vertical;
}

WorkspaceTabifyConfig::WorkspaceTabifyConfig(const QJsonObject &jsonObject) :
    GlobalConfigItem(jsonObject) {
    firstID = jsonObject.value("first").toString();
    secondID = jsonObject.value("second").toString();
}

WorkspaceLayout::WorkspaceLayout(const QJsonObject &jsonObject) : GlobalConfigItem(jsonObject) {
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

    QJsonArray splitArray = jsonObject.value("split").toArray();
    for(const QJsonValue &splitVal : std::as_const(splitArray)) {
        QJsonObject dockObj = splitVal.toObject();
        WorkspaceSplitConfig splitConfig(dockObj);
        splitConfigs << splitConfig;
    }

    QJsonArray tabifyArray = jsonObject.value("tabify").toArray();
    for(const QJsonValue &tabifyVal : std::as_const(tabifyArray)) {
        QJsonObject dockObj = tabifyVal.toObject();
        WorkspaceTabifyConfig tabifyConfig(dockObj);
        tabifyConfigs << tabifyConfig;
    }

    QJsonObject cornersObject = jsonObject.value("corners").toObject();
    corners.insert(Qt::TopLeftCorner, parseAreaString(cornersObject.value("topLeft").toString()));
    corners.insert(Qt::TopRightCorner, parseAreaString(cornersObject.value("topRight").toString()));
    corners.insert(Qt::BottomLeftCorner,
                   parseAreaString(cornersObject.value("bottomLeft").toString()));
    corners.insert(Qt::BottomRightCorner,
                   parseAreaString(cornersObject.value("bottomRight").toString()));
}

Qt::DockWidgetArea WorkspaceLayout::parseAreaString(const QString &string) {
    return string == "right"    ? Qt::RightDockWidgetArea
           : string == "left"   ? Qt::LeftDockWidgetArea
           : string == "top"    ? Qt::TopDockWidgetArea
           : string == "bottom" ? Qt::BottomDockWidgetArea
                                : Qt::NoDockWidgetArea;
}

WorkspaceConfig::WorkspaceConfig(const QJsonObject &jsonObject, const int &index) :
    GlobalConfigItem(jsonObject, index),
    layout(WorkspaceLayout(jsonObject.value("layout").toObject())) {
    name = jsonObject.value("name").toString();
    icon = jsonObject.value("icon").toString();
    defaultKeyboardShortcut = parseKeyboardShortcutConfigString(
        jsonObject.value("defaultKeyboardShortcut").toString());
}

WorkspaceConfig::WorkspaceConfig(const QString &id, const int &index) :
    GlobalConfigItem(id, index) {}

} // namespace ScheduleMaster
