#include "ActionShortcutMapper.h"

ActionShortcutMapper::ActionShortcutMapper() : QObject(nullptr) {}
ActionShortcutMapper::~ActionShortcutMapper() {}

void ActionShortcutMapper::init() {
    connect(LocalConfig::instance(), &LocalConfig::keyboardShortcutChanged, ActionShortcutMapper::instance(), &ActionShortcutMapper::onActionShortcutChanged);
}

ActionShortcutMapper *ActionShortcutMapper::instance() {
    static ActionShortcutMapper instance;
    return &instance;
}

void ActionShortcutMapper::map(QAction *action, const QString &shortcutID) {
    if(!action)
        return;

    if(_actionShortcutHash.contains(action)) {// don't add multiple times
        update(action, shortcutID);
        return;
    }

    QKeySequence shortcut = LocalConfig::keyboardShortcut(shortcutID); // get actual shortcut

    action->setShortcut(shortcut);

    if(!_shortcutActionsHash.contains(shortcutID)) // init hash value
        _shortcutActionsHash.insert(shortcutID, QList<QAction *>());

    _shortcutActionsHash[shortcutID] << action;
    _actionShortcutHash[action] = shortcutID;

    connect(action, &QObject::destroyed, instance(), &ActionShortcutMapper::onActionDestroyed);
}

void ActionShortcutMapper::update(QAction *action, const QString &shortcutID) {
    QString oldShortcut = _actionShortcutHash[action];
    if(oldShortcut == shortcutID)
        return;

    QKeySequence shortcut = LocalConfig::keyboardShortcut(shortcutID); // get actual shortcut

    action->setShortcut(shortcut);

    _actionShortcutHash[action] = shortcutID;

    _shortcutActionsHash[oldShortcut].removeAll(action);
    _shortcutActionsHash[shortcutID] << action;
}

void ActionShortcutMapper::remove(QAction *action) {
    QString shortcutID = _actionShortcutHash[action];
    _shortcutActionsHash[shortcutID].removeAll(action);
    _actionShortcutHash.remove(action);
    action->disconnect(instance());
}

void ActionShortcutMapper::onActionDestroyed(QObject *obj) {
    if(!obj)
        return;
    remove(static_cast<QAction *>(obj));
}

void ActionShortcutMapper::onActionShortcutChanged(const QString &id, const QKeySequence &keySequence) {
    if(!_shortcutActionsHash.contains(id))
        return;

    const QList<QAction *> actions = _shortcutActionsHash[id];
    for(QAction *action : actions)
        if(action->shortcut() != keySequence)
            action->setShortcut(keySequence);
}
