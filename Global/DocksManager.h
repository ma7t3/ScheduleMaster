#ifndef DOCKSMANAGER_H
#define DOCKSMANAGER_H

#include <QObject>
#include "GlobalConfigManager.h"

class DockConfig : public GlobalConfigItem {
public:
    DockConfig(const QJsonObject &jsonObject = QJsonObject());
    DockConfig(const QString &id);

    QString name, icon;
};

class DocksManager : public GlobalConfigManager<DocksManager, DockConfig> {
    friend class GlobalConfigManager<DocksManager, DockConfig>;
    Q_OBJECT
protected:
    explicit DocksManager(QObject *parent);

public:
};

#endif // DOCKSMANAGER_H
