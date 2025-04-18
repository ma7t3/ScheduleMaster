#ifndef ACTIONSHORTCUTMAPPER_H
#define ACTIONSHORTCUTMAPPER_H

#include <QObject>
#include <QHash>
#include <QAction>

#include "LocalConfig.h"

class ActionShortcutMapper : public QObject {
    Q_OBJECT
private:
    explicit ActionShortcutMapper();
    ~ActionShortcutMapper();

public:
    /// Initializes the ActionShortcutMapper class. Connects the LocalConfigs::keyboardShortcutChanged signal to the ActionShortcutMapper::onActionShortcutChanged slot.
    static void init();

    /// Returns the singleton instance of the ActionShortcutMapper class.
    static ActionShortcutMapper *instance();

    /// Maps an action to the given shortcut ID.
    static void map(QAction *action, const QString &shortcutID);

    /// Removes an action from the ActionShortcutMapper.
    static void remove(QAction *action);

    ActionShortcutMapper& operator=(const ActionShortcutMapper&) = delete;
    ActionShortcutMapper(const ActionShortcutMapper&) = delete;

protected slots:
    /// Updates an existing action with a new shortcutID
    static void update(QAction *action, const QString &shortcutID);

    /// Handles the destruction of an action. Removes the action from mapper
    static void onActionDestroyed(QObject *obj);

    /// Handles the change of a keyboard shortcut. Updates the action's shortcut.
    static void onActionShortcutChanged(const QString &id, const QKeySequence &keySequence);

private:
    static inline QHash<QString, QList<QAction *>> _shortcutActionsHash;
    static inline QHash<QAction *, QString> _actionShortcutHash;
};

#endif // ACTIONSHORTCUTMAPPER_H
