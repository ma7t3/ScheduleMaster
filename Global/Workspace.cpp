#include "Workspace.h"

Workspace::Workspace(QObject *parent) : QObject(parent) {
    _action = new QAction(this);
    setupAction();
}

Workspace::Workspace(const QString &name, const QIcon &icon, QObject *parent) : Workspace(parent) {
    setName(name);
    setIcon(icon);
}

Workspace::Workspace(QAction *action, QObject *parent) : QObject(parent) {
    _action = action;
    _name = _action->text();
    _icon = _action->icon();
    setupAction();
}

bool Workspace::active() const {
    return _action->isChecked();
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

void Workspace::activate() {
    _action->setChecked(true);
    emit activated(this);
}

void Workspace::deactivate() {
    _action->setChecked(false);
}

void Workspace::setupAction() {
    _action->setParent(this);
    _action->setCheckable(true);

    connect(_action, &QAction::triggered, this, [this](const bool &checked) {
        if(checked)
            activate();
        else
            _action->setChecked(true);
    });
}
