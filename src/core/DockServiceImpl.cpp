#include "DockServiceImpl.h"

#include "ApplicationInterfaceImpl.h"
#include "ActionServiceImpl.h"

#include "src/ui/widgets/Docks/DockWelcome.h"
#include "src/ui/widgets/Docks/DockNews.h"
#include "src/ui/widgets/Docks/DockUndoView.h"
#include "src/ui/widgets/Docks/DockBusstops.h"
#include "src/ui/widgets/Docks/DockLines.h"
#include "src/ui/widgets/Docks/DockRoutes.h"

#include <QUndoView>
#include <QVBoxLayout>

namespace ScheduleMaster::Core {

DockServiceImpl::DockServiceImpl(QObject *parent) : GlobalConfigServiceCRTP(parent, "Docks") {
    initRepository();

    const QList<DockConfig> itemList = repository()->items();
    for(const DockConfig &dock : itemList) {
        ActionConfig action(QString("view.docks.%1.toggle").arg(dock.id()));
        action.text                    = dock.name;
        action.description             = tr("Show/hide dock: %1").arg(dock.name);
        action.icon                    = dock.icon;
        action.breadcrumb              = {tr("View"), tr("Workspaces")};
        action.canHaveShortcut         = true;
        action.defaultKeyboardShortcut = dock.defaultKeyboardShortcut;
        ActionServiceImpl::instance()->registerAction(action);
    }
}

QList<DockConfig> DockServiceImpl::docks() const {
    return repository()->items();
}

QMap<QString, DockConfig> DockServiceImpl::docksMap() const {
    return repository()->itemsMap();
}

QStringList DockServiceImpl::dockIDs() const {
    return repository()->itemIDs();
}

DockConfig DockServiceImpl::dock(const QString &id) const {
    return repository()->item(id);
}

bool DockServiceImpl::dockExists(const QString &id) const {
    return repository()->itemExists(id);
}

bool DockServiceImpl::registerDock(const DockConfig &dock) const {
    return repository()->addItem(dock);
}

QDockWidget *DockServiceImpl::setDockWidget(const QString &dockID, QWidget *widget) {
    if(!dockExists(dockID))
        return nullptr;

    const DockConfig dockConfig = dock(dockID);
    QDockWidget *dockWidget = new QDockWidget(dockConfig.name, dynamic_cast<QWidget *>(app->mainWindow()));
    dockWidget->setObjectName(dockConfig.name + "Dock");
    dockWidget->setFloating(true);
    dockWidget->setVisible(false);
    dockWidget->setWidget(widget);
    _docks.insert(dockID, dockWidget);

    QAction *toggleAction = dockWidget->toggleViewAction();
    const QString actionID = QString("view.docks.%1.toggle").arg(dockConfig.id());
    ActionServiceImpl::instance()->addAction(toggleAction, actionID);
    ActionServiceImpl::instance()->setGlobalAction(actionID, toggleAction);
    _dockToggleActions.insert(dockConfig.id(), toggleAction);

    emit dockAdded(dockConfig.id(), dockWidget, toggleAction);
    return dockWidget;
}

QDockWidget *DockServiceImpl::dockWidget(const QString &dockID) const {
    return _docks.value(dockID, nullptr);
}

QList<QDockWidget *> DockServiceImpl::dockWidgets() const {
    return _docks.values();
}

QMap<QString, QDockWidget *> DockServiceImpl::dockWidgetsMap() const {
    return _docks;
}

void DockServiceImpl::loadStandardDocks() {
    QWidget *parent = dynamic_cast<QWidget *>(app->mainWindow());

    setDockWidget("welcome", new DockWelcome(parent));
    setDockWidget("news", new DockNews(parent));
    setDockWidget("undoView", new DockUndoView(parent));
    setDockWidget("busstops", new DockBusstops(parent));
    setDockWidget("lines", new DockLines(parent));
    setDockWidget("routes", new DockRoutes(parent));
}
}