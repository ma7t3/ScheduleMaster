#ifndef DOCKSERVICEIMPL_H
#define DOCKSERVICEIMPL_H

#include "IDockService.h"
#include "GlobalConfigService.h"
#include "GlobalConfigRepository.h"

class QAction;

namespace ScheduleMaster::Core {

using DockRepository = GlobalConfigRepositoryCRTP<DockConfig>;

class DockServiceImpl :
    public GlobalConfigServiceCRTP<DockRepository, DockServiceImpl>,
    public IDockService {
    Q_OBJECT
public:
    DockServiceImpl(QObject *parent);

    virtual QList<DockConfig> docks() const override;
    virtual QMap<QString, DockConfig> docksMap() const override;
    virtual QStringList dockIDs() const override;
    virtual DockConfig dock(const QString &id) const override;
    virtual bool dockExists(const QString &id) const override;
    virtual bool registerDock(const DockConfig &dock) const override;

    virtual QDockWidget *setDockWidget(const QString &dockID, QWidget *widget) override;
    virtual QDockWidget *dockWidget(const QString &dockID) const override;
    virtual QList<QDockWidget *> dockWidgets() const override;
    virtual QMap<QString, QDockWidget *> dockWidgetsMap() const override;

    void loadStandardDocks();

signals:
    void dockAdded(const QString &id, QDockWidget *dockWidget, QAction *toggleViewAction);

private:
    QMap<QString, QDockWidget *> _docks;
    QMap<QString, QAction *> _dockToggleActions;
};

} // namespace ScheduleMaster::Core

#endif // DOCKSERVICEIMPL_H
