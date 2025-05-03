#ifndef ACTIONSHORTCUTMAPPER_H
#define ACTIONSHORTCUTMAPPER_H

#include <QObject>
#include <QHash>

class QAction;

/**
 * @brief The ActionShortcutMapper class is used to connect QActions with their corresponding keyboard shortcuts.
 *
 * Since ScheduleMaster supports custom shortcuts that can be changed by the user and should update at runtime without restarting the application, an additional logic is needed to handle this.
 *
 * The class listens to the LocalConfigs::keyboardShortcutChanged signal and updates the action's shortcut accordingly.
 */
class ActionShortcutMapper : public QObject {
    Q_OBJECT
private:
    /// Private constructor for the signleton instance
    explicit ActionShortcutMapper();

    /// Private destructor for the signleton instance
    ~ActionShortcutMapper();

public:
    /**
     * Initializes the ActionShortcutMapper class.
     *
     * This connects the LocalConfigs::keyboardShortcutChanged signal to the ActionShortcutMapper::onActionShortcutChanged slot.
     */
    static void init();

    /// Returns the singleton instance of the ActionShortcutMapper class.
    static ActionShortcutMapper *instance();

    /**
     * @brief Maps an action to the given shortcut ID.
     * @param action The QAction pointer
     * @param shortcutID The shortcut ID that should be mapped to the action
     */
    static void map(QAction *action, const QString &shortcutID);

    /**
     * @brief Removes an action from the ActionShortcutMapper.
     * @param action The QAction pointer to remove
     */
    static void remove(QAction *action);

    ActionShortcutMapper& operator=(const ActionShortcutMapper&) = delete;
    ActionShortcutMapper(const ActionShortcutMapper&) = delete;

protected slots:
    /**
     * @brief Updates an existing action with a new shortcutID
     * @param action The QAction pointer to update
     * @param shortcutID The new shortcut ID to apply to action
     */
    static void update(QAction *action, const QString &shortcutID);

    /**
     * @brief Handles the destruction of an action. Removes the action from mapper
     * @param obj The QObject pointer to the action that was destroyed
     */
    static void onActionDestroyed(QObject *obj);

    /**
     * @brief Handles the change of a keyboard shortcut. Updates the action's shortcut.
     * @param id The shortcut ID
     * @param keySequence The new key sequence that is associated with the ID
     */
    static void onActionShortcutChanged(const QString &id, const QKeySequence &keySequence);

private:
    /// Maps a list of shortcut IDs to their corresponding actions.
    static inline QHash<QString, QList<QAction *>> _shortcutActionsHash;

    /// Maps every single action to its shortcut ID.
    static inline QHash<QAction *, QString> _actionShortcutHash;
};

#endif // ACTIONSHORTCUTMAPPER_H
