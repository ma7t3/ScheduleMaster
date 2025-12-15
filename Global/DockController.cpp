#include "DockController.h"

#include "Global/ActionController.h"
#include "src/ui/widgets/Docks/DockWelcome.h"
#include "src/ui/widgets/Docks/DockNews.h"
#include "src/ui/widgets/Docks/DockBusstops.h"
#include "src/ui/widgets/Docks/DockLines.h"
#include "src/ui/widgets/Docks/DockRoutes.h"

#include <QDockWidget>
#include <QUndoView>
#include <QVBoxLayout>

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

QList<QDockWidget *> DockController::dockList() {
    const QList<DockConfig> dockConfigs = DockManager::sortedItems();
    QList<QDockWidget *> resultList;
    for(const DockConfig &config : dockConfigs) {
        QString id = config.id();
        resultList << dock(id);
    }

    return resultList;
}

void DockController::loadStandardDocks() {
    addDock(DockManager::item("welcome"), new DockWelcome(static_cast<QWidget *>(parent())));
    addDock(DockManager::item("news"), new DockNews(static_cast<QWidget *>(parent())));

    QWidget *undoViewWidget = new QWidget(static_cast<QWidget *>(parent()));
    QLayout *undoLayout = new QVBoxLayout(undoViewWidget);
    undoLayout->addWidget(new QUndoView(static_cast<QWidget *>(parent())));
    undoViewWidget->setLayout(undoLayout);
    addDock(DockManager::item("undoView"), undoViewWidget);

    addDock(DockManager::item("busstops"), new DockBusstops(static_cast<QWidget *>(parent())));
    addDock(DockManager::item("lines"), new DockLines(static_cast<QWidget *>(parent())));
    addDock(DockManager::item("routes"), new DockRoutes(static_cast<QWidget *>(parent())));
}

QDockWidget *DockController::dock(const QString &id) {
    return _docks.value(id, nullptr);
}
