#ifndef FOLDERLOCATIONMANAGER_H
#define FOLDERLOCATIONMANAGER_H

#include <QObject>
#include <QStandardPaths>
#include <QApplication>

#include "Global/GlobalConfigManager.h"
#include "Global/SettingsManager.h"

struct FolderLocationConfig : public GlobalConfigItem {
public:
    FolderLocationConfig(const QJsonObject &jsonObject = QJsonObject());
    FolderLocationConfig(const QString &id);

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
};

#endif // FOLDERLOCATIONMANAGER_H
