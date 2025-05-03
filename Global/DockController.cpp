#include "DockController.h"

#include "Global/ActionShortcutMapper.h"
#include "Widgets/Docks/DockWelcome.h"

#include <QDockWidget>

DockController::DockController(QObject *parent) : QObject(parent) {}

void DockController::addDock(const DockConfig &dock, QWidget *contentWidget) {
    QDockWidget *dockWidget = new QDockWidget(dock.name, static_cast<QWidget *>(parent()));
    dockWidget->setObjectName(dock.name + "Dock");
    dockWidget->setFloating(true);
    dockWidget->setVisible(false);
    dockWidget->setWidget(contentWidget);
    _docks.insert(dock.id(), dockWidget);

    QAction *toggleAction = dockWidget->toggleViewAction();
    toggleAction->setIcon(QIcon(dock.icon));
    ActionShortcutMapper::map(toggleAction, QString("view.docks.%1.toggle").arg(dock.id()));
    _dockToggleActions.insert(dock.id(), toggleAction);

    emit dockAdded(dock.id(), dockWidget, toggleAction);
}

void DockController::loadStandardDocks() {
    addDock(DockManager::item("welcome"), new DockWelcome(static_cast<QWidget *>(parent())));
}
