#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>
#include <QKeySequence>

#include "GlobalConfigManager.h"

/**
 * @brief The ActionConfig struct represents the configuration for an action.
 *
 * It includes details such as text, tooltip, description, icon, and keyboard shortcut information.
 */
struct ActionConfig : public GlobalConfigItem {
    /**
     * @brief Constructs an ActionConfig object from a JSON object.
     * @param jsonObject The JSON object containing the action configuration.
     * @param index The index of the action.
     */
    ActionConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);

    /**
     * @brief Constructs an ActionConfig object with a given ID.
     * @param id The ID of the action.
     * @param index The index of the action.
     */
    ActionConfig(const QString &id, const int &index = 0) : GlobalConfigItem(id, index) {}

    bool canHaveShortcut; ///< Indicates if the action can have a shortcut.
    QString text; ///< The text of the action.
    QString tooltip; ///< The tooltip of the action.
    QString description; ///< The description of the action.
    QString icon; ///< The icon of the action.
    QStringList breadcrumb; ///< The breadcrumb path for the action.
    QKeySequence defaultKeyboardShortcut; ///< The default keyboard shortcut for the action.
};

/**
 * @brief The ActionManager class manages actions and their configurations.
 *
 * It provides functionalities to initialize actions, manage keyboard shortcuts,
 * and import/export action configurations.
 */
class ActionManager : public GlobalConfigManager<ActionManager, ActionConfig> {
    friend class GlobalConfigManager<ActionManager, ActionConfig>;
    Q_OBJECT
    Q_DISABLE_COPY(ActionManager)

protected:
    /**
     * @brief Constructs an ActionManager object.
     * @param parent The parent QObject.
     */
    explicit ActionManager(QObject *parent) : GlobalConfigManager(parent) {}

public:
    /**
     * @brief Initializes the ActionManager.
     */
    static void init();

    /**
     * @brief Checks if a shortcut is the default for a given action ID.
     * @param id The action ID.
     * @param sequence The keyboard shortcut sequence to check.
     * @return True if the shortcut is the default, false otherwise.
     */
    static bool shortcutIsDefault(const QString &id, const QKeySequence &sequence);

    /**
     * @brief Retrieves the keyboard shortcut for a given action ID.
     * @param actionID The ID of the action.
     * @return The keyboard shortcut for the action.
     */
    static QKeySequence keyboardShortcut(const QString &actionID);

    /**
     * @brief Sets a keyboard shortcut for a given action ID.
     * @param actionID The ID of the action.
     * @param shortcut The keyboard shortcut to set.
     */
    static void setKeyboardShortcut(const QString &actionID, const QKeySequence shortcut);

    /**
     * @brief Imports keyboard shortcuts from a JSON array.
     * @param jsonArray The JSON array containing keyboard shortcuts.
     */
    static void importKeyboardShortcuts(const QJsonArray &jsonArray);

    /**
     * @brief Exports keyboard shortcuts to a JSON array.
     * @return A JSON array containing the keyboard shortcuts.
     */
    static QJsonArray exportKeyboardShortcuts();

    /**
     * @brief Adds a new action configuration.
     * @param actionConfig The action configuration to add.
     * @return True if the action was added successfully, false otherwise.
     */
    static bool addItem(const ActionConfig &);

signals:
    /**
     * @brief Emitted when a keyboard shortcut is changed.
     * @param keyboardShortcutID The ID of the keyboard shortcut.
     * @param shortcut The new keyboard shortcut.
     */
    void keyboardShortcutChanged(const QString &keyboardShortcutID, const QKeySequence &shortcut);
};

#endif // ACTIONMANAGER_H

