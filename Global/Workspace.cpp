#include "Workspace.h"

#include "Global/ActionController.h"
#include "Global/WorkspaceManager.h"
#include "src/ui/widgets/Docks/DockAbstract.h"

#include "Global/DockController.h"

#include <QApplication>
#include <QIcon>
#include <QAction>
#include <QJsonObject>
#include <QMainWindow>
#include <QDockWidget>
#include <QSplitter>

Workspace::Workspace(const QString &id, QObject *parent) : QObject(parent), _id(id) {
    _action = new QAction(this);
    setupAction();
}

Workspace::Workspace(const QString &id, const QString &name, const QString &icon, QObject *parent) : Workspace(id, parent) {
    setName(name);
    setIcon(icon);
}

Workspace::Workspace(const QString &id, QAction *action, QObject *parent) : QObject(parent), _id(id) {
    _action = action;
    _name = _action->text();
    setupAction();
}

Workspace::Workspace(const WorkspaceConfig &config, QObject *parent) : QObject(parent) {
    _id   = config.id();
    _name = config.name;
    _icon = config.icon;
    _layout = config.layout;
    _action = new QAction(IconController::icon(_icon), _name, this);
    setupAction();
}

bool Workspace::active() const {
    return _action->isChecked();
}

QString Workspace::id() const {
    return _id;
}

QString Workspace::name() const {
    return _name;
}

void Workspace::setName(const QString &newName) {
    _name = newName;
    _action->setText(newName);
}

QString Workspace::icon() const {
    return _icon;
}

void Workspace::setIcon(const QString &newIcon) {
    _icon = newIcon;
    _action->setIcon(IconController::icon(_icon));
}

QAction *Workspace::action() const {
    return _action;
}

QByteArray Workspace::lastWindowState() const {
    return _lastWindowState;
}

void Workspace::activate() {
    _action->setChecked(true);
    qDebug().noquote() << "Workspace activated:" << _id;

    emit activated(this);
}

void Workspace::deactivate() {
    if(_action->isChecked())
        _lastWindowState = mainWindow()->saveState();

    _action->setChecked(false);
}

void Workspace::apply() {
    if(!_lastWindowState.isEmpty()) {
        hideAllDocks();
        mainWindow()->restoreState(_lastWindowState);
    } else
        restore();
}

void Workspace::restore() {
    _lastWindowState.clear();
    hideAllDocks();
    QMap<QString, QDockWidget *> docks = DockController::docks();

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

        mainWindow()->splitDockWidget(first, second, split.orientation);
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

        mainWindow()->tabifyDockWidget(first, second);
    }

    for(const WorkspaceResizeConfig &resize : std::as_const(_layout.resizeConfigs)) {
        int refSize = resize.orientation == Qt::Horizontal ? mainWindow()->width() : mainWindow()->height();
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
        mainWindow()->resizeDocks(currentDocks, calculatedSizes, resize.orientation);
    }

    mainWindow()->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    mainWindow()->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    mainWindow()->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    mainWindow()->setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    for(const Qt::Corner &corner : _layout.corners.keys()) {
        Qt::DockWidgetArea area = _layout.corners.value(corner);

        if((corner == Qt::TopLeftCorner || corner == Qt::BottomLeftCorner) && area == Qt::NoDockWidgetArea)
            area = Qt::LeftDockWidgetArea;
        else if((corner == Qt::TopRightCorner || corner == Qt::BottomRightCorner) && area == Qt::NoDockWidgetArea)
            area = Qt::RightDockWidgetArea;

        mainWindow()->setCorner(corner, area);
    }

    emit restored(this);
}

void Workspace::setupAction() {
    _action->setParent(this);
    _action->setCheckable(true);
    ActionController::addAsGlobalAction(_action, QString("view.workspaces.%1.activate").arg(_id));

    _action->setText(_name);

    connect(_action, &QAction::triggered, this, [this](const bool &checked) {
        if(checked)
            activate();
        else {
            _action->setChecked(true);
            restore();
        }
    });
}

QMainWindow *Workspace::mainWindow() {
    return static_cast<QMainWindow *>(QObject::parent()->parent());
}

void Workspace::hideAllDocks() {
    QMap<QString, QDockWidget *> docks = DockController::docks();
    for(QDockWidget *dock : std::as_const(docks)) {
        dock->hide();
        dock->setFloating(true);
    }
}
