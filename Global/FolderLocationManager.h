#ifndef FOLDERLOCATIONMANAGER_H
#define FOLDERLOCATIONMANAGER_H

#include "Global/GlobalConfigManager.h"

#include <QObject>

struct FolderLocationConfig : public GlobalConfigItem {
public:
    FolderLocationConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    FolderLocationConfig(const QString &id, const int &index = 0);

    QString name, icon;
    QStringList defaultPaths;
    bool multiple, requiresRestart;

protected:
    static QString resolvePathPlaceholders(QString path);
};


class FolderLocationManager : public GlobalConfigManager<FolderLocationManager, FolderLocationConfig> {
    friend class GlobalConfigManager<FolderLocationManager, FolderLocationConfig>;
    Q_OBJECT
protected:
    explicit FolderLocationManager(QObject *parent);

public:
    static QMap<QString, QStringList> currentFolderLocations();
    static QStringList currentFolderLocationPaths(const QString &folderLocationID);

    static void setCurrentFolderLocationPaths(const QString &folderLocationID, const QStringList &paths);
    static void setCurrentFolderLocations(const QMap<QString, QStringList> &folderLocations);

signals:
    void currentFolderLocationPathsChanged(const QString &folderLocationID, const QStringList &paths);
};

#endif // FOLDERLOCATIONMANAGER_H
