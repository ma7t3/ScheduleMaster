#ifndef DOCKMANAGER_H
#define DOCKMANAGER_H

#include "Global/GlobalConfigManager.h"

#include <QObject>
#include <QKeySequence>

struct DockConfig : public GlobalConfigItem {
public:
    DockConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    DockConfig(const QString &id, const int &index = 0);

    QString name, icon;
    QKeySequence defaultKeyboardShortcut;
};

class DockManager : public GlobalConfigManager<DockManager, DockConfig> {
    friend class GlobalConfigManager<DockManager, DockConfig>;
    Q_OBJECT
protected:
    explicit DockManager(QObject *parent);

public:
};

#endif // DOCKMANAGER_H
