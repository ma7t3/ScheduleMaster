#ifndef IDOCKSERVICE_H
#define IDOCKSERVICE_H

#include "ScheduleMaster_global.h"
#include "GlobalConfigItem.h"

#include <QKeySequence>

class QDockWidget;

namespace ScheduleMaster {

class DockConfig : public GlobalConfigItem {
public:
    DockConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    DockConfig(const QString &id, const int &index = 0);

    QString name;
    QString icon;
    QKeySequence defaultKeyboardShortcut;
};


class SCHEDULEMASTERINTERFACE_EXPORT IDockService {
public:
    virtual ~IDockService() = default;

    virtual QList<DockConfig> docks() const = 0;
    virtual QMap<QString, DockConfig> docksMap() const = 0;
    virtual QStringList dockIDs() const = 0;
    virtual DockConfig dock(const QString &id) const = 0;
    virtual bool dockExists(const QString &id) const = 0;
    virtual bool registerDock(const DockConfig &dock) = 0;

    virtual QDockWidget *setDockWidget(const QString &dockID, QWidget *widget) = 0;
    virtual QDockWidget *dockWidget(const QString &dockID) const = 0;
    virtual QList<QDockWidget *> dockWidgets() const = 0;
    virtual QMap<QString, QDockWidget *> dockWidgetsMap() const = 0;

};

} // namespace ScheduleMaster

#endif // IDOCKSERVICE_H
