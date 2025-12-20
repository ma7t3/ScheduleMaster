#ifndef IFOLDERLOCATIONSERVICE_H
#define IFOLDERLOCATIONSERVICE_H

#include "ScheduleMaster_global.h"
#include "GlobalConfigItem.h"

namespace ScheduleMaster {

struct FolderLocationConfig : public GlobalConfigItem {
public:
    FolderLocationConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    FolderLocationConfig(const QString &id, const int &index = 0) : GlobalConfigItem(id, index) {}

    QString name;
    QString icon;
    QStringList defaultPaths;
    bool multiple;
    bool requiresRestart;

protected:
    static QString resolvePathPlaceholders(QString path);
};

class SCHEDULEMASTERINTERFACE_EXPORT IFolderLocationService {
public:
    virtual ~IFolderLocationService() = default;

    virtual QMap<QString, QStringList> currentFolderLocations() const = 0;
    virtual QStringList currentFolderLocationPaths(const QString &folderLocationID) const = 0;
    virtual void setCurrentFolderLocationPaths(const QString &folderLocationID, const QStringList &paths) = 0;
    virtual void setCurrentFolderLocations(const QMap<QString, QStringList> &folderLocations) = 0;
    virtual bool folderLocationExists(const QString &id) const = 0;
    virtual FolderLocationConfig folderLocation(const QString &id) const = 0;
    virtual QList<FolderLocationConfig> folderLocations() const = 0;
    virtual bool registerFolderLocations(FolderLocationConfig folderLocation) = 0;

};

}

#endif // IFOLDERLOCATIONSERVICE_H
