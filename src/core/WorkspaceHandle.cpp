#include "WorkspaceHandle.h"

#include "namespace.h"
#include "core/IconServiceImpl.h"
#include "core/ActionServiceImpl.h"
#include "core/DockServiceImpl.h"

#include "src/ui/widgets/Docks/DockAbstract.h"


#include <QApplication>
#include <QIcon>
#include <QAction>
#include <QJsonObject>
#include <QMainWindow>
#include <QDockWidget>
#include <QSplitter>

namespace ScheduleMaster::Core {

WorkspaceHandle::WorkspaceHandle(const WorkspaceConfig &config, QMainWindow *mainWindow,
                                 QObject *parent) : QObject(parent), _mainWindow{mainWindow} {
    _id   = config.id();
    _name = config.name;
    _index = config.index();
    _layout = config.layout;
    setupAction();
}

QString WorkspaceHandle::name() const {
    return _name;
}

int WorkspaceHandle::index() const {
    return _index;
}

QString WorkspaceHandle::id() const {
    return _id;
}

bool WorkspaceHandle::isActive() const {
    return _action->isChecked();
}

QAction *WorkspaceHandle::action() const {
    return _action;
}

void WorkspaceHandle::setActive(bool active) {
    _action->setChecked(active);
}

void WorkspaceHandle::apply() {
    if(!_lastWindowState.isEmpty()) {
        _mainWindow->restoreState(_lastWindowState);
        return;
    }

    resetWindowLayout();
    const QMap<QString, QDockWidget *> docks = DockServiceImpl::instance()->dockWidgetsMap();

    for(const WorkspaceDockConfig &config : std::as_const(_layout.dockConfigs)) {
        QDockWidget *widget = docks.value(config.id());
        if(!widget) {
            qWarning() << "Dock with id" << config.id() << "cannot be placed in workspace" << _id << "because it does not exist.";
            continue;
        }
        widget->setDockLocation(config.area);
        widget->setFloating(config.area == Qt::NoDockWidgetArea);
        widget->setVisible(config.visible);
    }

    for(const WorkspaceSplitConfig &split : std::as_const(_layout.splitConfigs)) {
        QDockWidget *first  = docks.value(split.firstID);
        QDockWidget *second = docks.value(split.secondID);

        if(!first || !second) {
            qWarning() << "Cannot split docks in workspace" << _id << "because one of the docks does not exist.";
            continue;
        }

        if(first->isFloating()) {
            qWarning() << "Dock" << first->objectName() << "is floating, cannot split it.";
            continue;
        }

        second->setDockLocation(first->dockLocation());
        second->setFloating(false);

        _mainWindow->splitDockWidget(first, second, split.orientation);
    }

    for(const WorkspaceTabifyConfig &tabify : std::as_const(_layout.tabifyConfigs)) {
        QDockWidget *first  = docks.value(tabify.firstID);
        QDockWidget *second = docks.value(tabify.secondID);

        if(!first || !second) {
            qWarning() << "Cannot tabify docks in workspace" << _id << "because one of the docks does not exist.";
            continue;
        }

        if(first->isFloating()) {
            qWarning() << "Dock" << first->objectName() << "is floating, cannot tabify it.";
            continue;
        }

        second->setDockLocation(first->dockLocation());
        second->setFloating(false);

        _mainWindow->tabifyDockWidget(first, second);
    }

    for(const WorkspaceResizeConfig &resize : std::as_const(_layout.resizeConfigs)) {
        int refSize = resize.orientation == Qt::Horizontal ? _mainWindow->width() : _mainWindow->height();
        QList<QDockWidget *> currentDocks;
        for(const QString &dockID : std::as_const(resize.dockIDs)) {
            QDockWidget *dock = docks.value(dockID);
            if(!dock)
                continue;

            currentDocks << dock;
        }

        QList<int> calculatedSizes;
        for(const float &value : resize.sizes) {
            calculatedSizes << static_cast<int>(value * refSize);
        }
        _mainWindow->resizeDocks(currentDocks, calculatedSizes, resize.orientation);
    }

    _mainWindow->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    _mainWindow->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    _mainWindow->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    _mainWindow->setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    for(const Qt::Corner &corner : _layout.corners.keys()) {
        Qt::DockWidgetArea area = _layout.corners.value(corner);

        if((corner == Qt::TopLeftCorner || corner == Qt::BottomLeftCorner) && area == Qt::NoDockWidgetArea)
            area = Qt::LeftDockWidgetArea;
        else if((corner == Qt::TopRightCorner || corner == Qt::BottomRightCorner) && area == Qt::NoDockWidgetArea)
            area = Qt::RightDockWidgetArea;

        _mainWindow->setCorner(corner, area);
    }
}

void WorkspaceHandle::saveWindowState() {
    _lastWindowState = _mainWindow->saveState();
}

void WorkspaceHandle::clearWindowState() {
    _lastWindowState.clear();
}

void WorkspaceHandle::setupAction() {
    _action = new QAction(this);
    _action->setParent(this);
    _action->setCheckable(true);
    const QString actionID = QString("view.workspaces.%1.activate").arg(_id);
    SM::ActionServiceImpl::instance()->addAction(_action, actionID);
    SM::ActionServiceImpl::instance()->setGlobalAction(actionID, _action);
    _action->setText(_name);
}

void WorkspaceHandle::resetWindowLayout() {
    const QList<QDockWidget *> docks = DockServiceImpl::instance()->dockWidgets();
    for(QDockWidget *dock : std::as_const(docks)) {
        dock->hide();
        dock->setFloating(true);
    }
}

}