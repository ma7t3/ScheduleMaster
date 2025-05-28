#include "ActionController.h"

#include "ActionManager.h"
#include "IconController.h"

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
}

template<typename T>
T *ActionController::add(T *widget, const QString &actionID) {    
    if(!ActionManager::itemExists(actionID)) {
        qWarning() << "cannot add actoin for actionID" << actionID << "- action does not exist in ActionManager.";
        return widget;
    }

    ActionConfig actionConfig = ActionManager::item(actionID);

    if constexpr(std::is_base_of<QAbstractButton, T>::value) {
        QAbstractButton *button = static_cast<QAbstractButton *>(widget);
        _buttons.insert(button, actionID);
        connect(button, &QObject::destroyed, instance(), [button](){ _buttons.remove(button); });

        button->setText(actionConfig.text);
        button->setToolTip(actionConfig.tooltip);
        button->setIcon(IconController::icon(actionConfig.icon));

        QKeySequence shortcut = ActionManager::keyboardShortcut(actionID);
        button->setShortcut(shortcut);

    } else if constexpr(std::is_base_of<QAction, T>::value) {
        QAction *action = static_cast<QAction *>(widget);
        _actions.insert(action, actionID);
        connect(action, &QObject::destroyed, instance(), [action](){ _actions.remove(action); });

        action->setText(actionConfig.text);
        action->setToolTip(actionConfig.tooltip);
        action->setIcon(IconController::icon(actionConfig.icon));

        QKeySequence shortcut = ActionManager::keyboardShortcut(actionID);
        action->setShortcut(shortcut);

    } else {
        qWarning() << "Cannot add action to ActionController. Unsupported type:" << typeid(T).name();
    }

    return widget;
}

template<typename T>
T *ActionController::remove(T *widget) {
    if constexpr(std::is_base_of<QAbstractButton, T>::value)
        _buttons.remove(widget);

    else if constexpr(std::is_base_of<QAction, T>::value)
        _actions.remove(widget);
}
