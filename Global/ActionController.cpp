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

QAction *ActionController::createAction(const QString &actionID, const ActionComponents &components, QObject *parent) {
    return add(new QAction(parent), actionID, components);
}

QPushButton *ActionController::createPushButton(const QString &actionID, const ActionComponents &components, QWidget *parent) {
    return add(new QPushButton(parent), actionID, components);
}

QToolButton *ActionController::createToolButton(const QString &actionID, const ActionComponents &components, QWidget *parent) {
    return add(new QToolButton(parent), actionID, components);
}

QCommandLinkButton *ActionController::createCommandLinkButton(const QString &actionID, const ActionComponents &components, QWidget *parent) {
    return add(new QCommandLinkButton(parent), actionID, components);
}

QStringList ActionController::globalActionIDs() {
    return _globalActions.keys();
}

void ActionController::onIconSetChanged() {
    for(QAction *action : _actions.keys()) {
        if(!_actions[action].second.testFlag(IconComponent))
            continue;

        QString actionID = _actions[action].first;
        ActionConfig actionConfig = ActionManager::item(actionID);
        action->setIcon(IconController::icon(actionConfig.icon));
    }

    for(QMenu *menu : _menus.keys()) {
        if(!_menus[menu].second.testFlag(IconComponent))
            continue;

        QString actionID = _menus[menu].first;
        ActionConfig actionConfig = ActionManager::item(actionID);
        menu->setIcon(IconController::icon(actionConfig.icon));
    }

    for(QAbstractButton *button : _buttons.keys()) {
        if(!_buttons[button].second.testFlag(IconComponent))
            continue;

        QString actionID = _buttons[button].first;
        ActionConfig actionConfig = ActionManager::item(actionID);
        button->setIcon(IconController::icon(actionConfig.icon));
    }
}

void ActionController::onActionShortcutChanged(const QString &keyboardShortcutID, const QKeySequence shortcut) {
    for(QAction *action : _actions.keys()) {
        if(_actions[action].first != keyboardShortcutID || !_actions[action].second.testFlag(ShortcutComponent))
            continue;

        if(action->shortcut() != shortcut)
            action->setShortcut(shortcut);
    }

    for(QAbstractButton *button : _buttons.keys()) {
        if(_buttons[button].first != keyboardShortcutID || !_buttons[button].second.testFlag(ShortcutComponent))
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
