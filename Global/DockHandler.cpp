#include "DockHandler.h"

DockHandler::DockHandler(QObject *parent) : QObject(parent) {}

void DockHandler::addDock(const DockConfig &dock, QWidget *contentWidget) {
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

void DockHandler::loadStandardDocks() {
    addDock(DocksManager::item("welcome"), new DockWelcome(static_cast<QWidget *>(parent())));
}
