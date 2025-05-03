#include "Workspace.h"

Workspace::Workspace(const QString &id, QObject *parent) : QObject(parent), _id(id) {
    _action = new QAction(this);
    setupAction();
}

Workspace::Workspace(const QString &id, const QString &name, const QIcon &icon, QObject *parent) : Workspace(id, parent) {
    setName(name);
    setIcon(icon);
}

Workspace::Workspace(const QString &id, QAction *action, QObject *parent) : QObject(parent), _id(id) {
    _action = action;
    _name = _action->text();
    _icon = _action->icon();
    setupAction();
}

Workspace::Workspace(const WorkspaceConfig &config, QObject *parent) : QObject(parent) {
    _id   = config.id();
    _name = config.name;
    _icon = config.icon;
    _action = new QAction(_icon, _name, this);
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

QIcon Workspace::icon() const {
    return _icon;
}

void Workspace::setIcon(const QIcon &newIcon) {
    _icon = newIcon;
    _action->setIcon(_icon);
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
    if(!_lastWindowState.isEmpty())
        mainWindow()->restoreState(_lastWindowState);
    else
        restore();
}

void Workspace::restore() {
    _lastWindowState.clear();
    // TODO: restore default layout
    emit restored(this);
}

void Workspace::setupAction() {
    _action->setParent(this);
    _action->setCheckable(true);
    _action->setText(_name);
    _action->setIcon(_icon);

    ActionShortcutMapper::map(_action, QString("view.workspaces.%1.activate").arg(_id));

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
