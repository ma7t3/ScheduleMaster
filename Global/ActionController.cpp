#include "ActionController.h"

#include "Global/StyleHandler.h"

GlobalActionWrapper::GlobalActionWrapper(QObject *widget) : widget(widget) {
    if(widget) {
        QAction *action = qobject_cast<QAction *>(widget);
        QAbstractButton *button = qobject_cast<QAbstractButton *>(widget);

        if(action)
            shortcut = action->shortcut();

        if(button)
            shortcut = button->shortcut();
    }
}

void GlobalActionWrapper::execute() {
    QAction *action = qobject_cast<QAction *>(widget);
    QAbstractButton *button = qobject_cast<QAbstractButton *>(widget);

    if(action)
        action->trigger();

    if(button)
        button->click();
}

void ActionController::init() {
    connect(IconController::instance(), &IconController::currentIconSetChanged,  ActionController::instance(), &ActionController::onIconSetChanged);
    connect(StyleHandler::instance(),   &StyleHandler::styleChanged,             ActionController::instance(), &ActionController::onIconSetChanged);
    connect(ActionManager::instance(),  &ActionManager::keyboardShortcutChanged, ActionController::instance(), &ActionController::onActionShortcutChanged);
}

QAction *ActionController::createAction(const QString &actionID, const ActionComponents &components,
                                        QObject *parent) {
    return add(new QAction(parent), actionID, components);
}

QPushButton *ActionController::createPushButton(const QString &actionID,
                                                const ActionComponents &components,
                                                QWidget *parent) {
    return add(new QPushButton(parent), actionID, components);
}

QToolButton *ActionController::createToolButton(const QString &actionID,
                                                const ActionComponents &components,
                                                QWidget *parent) {
    return add(new QToolButton(parent), actionID, components);
}

QCommandLinkButton *ActionController::createCommandLinkButton(const QString &actionID,
                                                              const ActionComponents &components,
                                                              QWidget *parent) {
    return add(new QCommandLinkButton(parent), actionID, components);
}

QStringList ActionController::globalActionIDs() {
    return _globalActions.keys();
}

GlobalActionWrapper ActionController::globalAction(const QString &actionID) {
    return _globalActions.value(actionID);
}

void ActionController::setGlobalAction(const QString &actionID, QObject *widget) {
    if(_globalActions.contains(actionID))
        instance()->disconnect(qobject_cast<QObject *>(_globalActions[actionID].widget));

    _globalActions.insert(actionID, widget);
    connect(widget, &QObject::destroyed, instance(), [actionID]() {
        _globalActions.remove(actionID);
    });
}

void ActionController::onIconSetChanged() {
    for(auto it = _actions.begin(); it != _actions.end(); ++it) {
        if(!it.value().second.testFlag(IconComponent))
            continue;

        QString actionID = it.value().first;
        ActionConfig actionConfig = ActionManager::item(actionID);
        it.key()->setIcon(IconController::icon(actionConfig.icon));
    }

    for(auto it = _menus.begin(); it != _menus.end(); ++it) {
        if(!it.value().second.testFlag(IconComponent))
            continue;

        QString actionID = it.value().first;
        ActionConfig actionConfig = ActionManager::item(actionID);
        it.key()->setIcon(IconController::icon(actionConfig.icon));
    }

    for(auto it = _buttons.begin(); it != _buttons.end(); ++it) {
        if(!it.value().second.testFlag(IconComponent))
            continue;

        QString actionID = it.value().first;
        ActionConfig actionConfig = ActionManager::item(actionID);
        it.key()->setIcon(IconController::icon(actionConfig.icon));
    }
}

void ActionController::onActionShortcutChanged(const QString &keyboardShortcutID,
                                               const QKeySequence shortcut) {
    for(auto it = _actions.begin(); it != _actions.end(); ++it) {
        if(it.value().first != keyboardShortcutID
           || !it.value().second.testFlag(ShortcutComponent))
            continue;

        if(it.key()->shortcut() != shortcut)
            it.key()->setShortcut(shortcut);
    }

    for(auto it = _buttons.begin(); it != _buttons.end(); ++it) {
        if(it.value().first != keyboardShortcutID
           || !it.value().second.testFlag(ShortcutComponent))
            continue;

        if(it.key()->shortcut() != shortcut)
            it.key()->setShortcut(shortcut);
    }

    if(_globalActions.contains(keyboardShortcutID))
        _globalActions[keyboardShortcutID].shortcut = shortcut;
}
