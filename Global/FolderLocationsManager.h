#ifndef FOLDERLOCATIONSMANAGER_H
#define FOLDERLOCATIONSMANAGER_H

#include <QObject>
#include <QStandardPaths>
#include <QApplication>

#include "Global/GlobalConfigManager.h"
#include "Global/GlobalConfig.h"

class FolderLocation : public GlobalConfigItem {
public:
    FolderLocation(const QJsonObject &jsonObject = QJsonObject());
    FolderLocation(const QString &id);

    QString name, icon;
    QStringList defaultPaths;
    bool multiple, requiresRestart;

protected:
    static QString resolvePathPlaceholders(QString path);
};


class FolderLocationsManager : public GlobalConfigManager<FolderLocationsManager, FolderLocation> {
    friend class GlobalConfigManager<FolderLocationsManager, FolderLocation>;
    Q_OBJECT
protected:
    explicit FolderLocationsManager(QObject *parent);
};

#endif // FOLDERLOCATIONSMANAGER_H
