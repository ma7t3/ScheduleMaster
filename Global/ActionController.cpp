#include "ActionController.h"

ActionController::ActionController(QObject *parent) : QObject(parent) {
    connect(IconController::instance(), &IconController::currentIconSetChanged, this, &ActionController::onIconSetChanged);
    connect(ActionManager::instance(), &ActionManager::keyboardShortcutChanged, this, &ActionController::onActionShortcutChanged);
}

void ActionController::init() {
    instance();
}

ActionController *ActionController::instance() {
    static ActionController instance(nullptr);
    return &instance;
}

QAction *ActionController::createAction(const QString &actionID, QObject *parent) {
    return add(new QAction(parent), actionID);
}

QPushButton *ActionController::createPushButton(const QString &actionID, QWidget *parent) {
    return add(new QPushButton(parent), actionID);
}

QToolButton *ActionController::createToolButton(const QString &actionID, QWidget *parent) {
    return add(new QToolButton(parent), actionID);
}

QCommandLinkButton *ActionController::createCommandLinkButton(const QString &actionID, QWidget *parent) {
    return add(new QCommandLinkButton(parent), actionID);
}

void ActionController::onIconSetChanged() {
    for(QAction *action : _actions.keys()) {
        QString actionID = _actions[action];
        ActionConfig actionConfig = ActionManager::item(actionID);
        action->setIcon(IconController::icon(actionConfig.icon));
    }

    for(QMenu *menu : _menus.keys()) {
        QString actionID = _menus[menu];
        ActionConfig actionConfig = ActionManager::item(actionID);
        menu->setIcon(IconController::icon(actionConfig.icon));
    }

    for(QAbstractButton *button : _buttons.keys()) {
        QString actionID = _buttons[button];
        ActionConfig actionConfig = ActionManager::item(actionID);
        button->setIcon(IconController::icon(actionConfig.icon));
    }
}

void ActionController::onActionShortcutChanged(const QString &keyboardShortcutID, const QKeySequence shortcut) {
    for(QAction *action : _actions.keys()) {
        if(_actions.value(action) != keyboardShortcutID)
            continue;

        if(action->shortcut() != shortcut)
            action->setShortcut(shortcut);
    }

    for(QAbstractButton *button : _buttons.keys()) {
        if(_buttons.value(button) != keyboardShortcutID)
            continue;

        if(button->shortcut() != shortcut)
            button->setShortcut(shortcut);
    }
}



template<typename T>
T *ActionController::remove(T *widget) {
    if constexpr(std::is_base_of<QAbstractButton, T>::value)
        _buttons.remove(widget);

    else if constexpr(std::is_base_of<QAction, T>::value)
        _actions.remove(widget);
}
