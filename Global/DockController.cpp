#include "DockController.h"

#include "Global/ActionController.h"
#include "Widgets/Docks/DockWelcome.h"
#include "Widgets/Docks/DockNews.h"

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
    QString actionText = toggleAction->text();
    ActionController::addAsGlobalAction(toggleAction, QString("view.docks.%1.toggle").arg(dock.id()));
    toggleAction->setText(actionText);
    _dockToggleActions.insert(dock.id(), toggleAction);

    emit dockAdded(dock.id(), dockWidget, toggleAction);
}

QMap<QString, QDockWidget *> DockController::docks() {
    return _docks;
}

void DockController::loadStandardDocks() {
    addDock(DockManager::item("welcome"), new DockWelcome(static_cast<QWidget *>(parent())));
    addDock(DockManager::item("news"), new DockNews(static_cast<QWidget *>(parent())));
}
