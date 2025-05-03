#ifndef DOCKMANAGER_H
#define DOCKMANAGER_H

#include "Global/GlobalConfigManager.h"

#include <QObject>

struct DockConfig : public GlobalConfigItem {
public:
    DockConfig(const QJsonObject &jsonObject = QJsonObject());
    DockConfig(const QString &id);

    QString name, icon;
};

class DockManager : public GlobalConfigManager<DockManager, DockConfig> {
    friend class GlobalConfigManager<DockManager, DockConfig>;
    Q_OBJECT
protected:
    explicit DockManager(QObject *parent);

public:
};

#endif // DOCKMANAGER_H
