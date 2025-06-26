#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QPointer>

#include "GlobalConfigManager.h"

class SettingsItem : public GlobalConfigItem {
public:
    SettingsItem(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    SettingsItem(const QString &id, const int &index = 0);

    QString description;
    QVariant defaultValue;
    QMetaType::Type type = QMetaType::Void, groupContentType = QMetaType::Void;
    bool isGroup = false, requiresRestart = false;
};

/**
 * @brief The SettingsManager class manages all SettingsItems registered to the application as well as their current values.
 *
 * It provides a simple interface to read and write settings, as well as to register new settings items at runtime.
 */
class SettingsManager : public GlobalConfigManager<SettingsManager, SettingsItem> {
    friend class GlobalConfigManager<SettingsManager, SettingsItem>;
    Q_OBJECT
protected:
    explicit SettingsManager(QObject *parent);
public:

    using FilterFunction = std::function<bool(const QString &settingID)>;
    using CallbackFunction = std::function<void(const QString &settingID, const QVariant &value)>;

    /**
     * @brief Returns the value of the given setting ID.
     *
     * If the setting ID is registered, the value is converted to the correct type if possible and the default value will be returned if the setting is not set.
     *
     * If the setting ID is not registered, the value is returned as is or an empty QVariant if the setting is not set.
     * @param id The setting ID
     * @return The current value
     */
    static QVariant value(const QString &id);

    /**
     * @brief Sets the value of the given setting ID to value.
     *
     * If the setting ID is registered, the value is converted to the correct type if possible.
     * @param id The setting ID
     * @param value The value to set
     * @return The value that was set
     */
    static QVariant setValue(const QString &id, const QVariant &value);

    /**
     * @brief Removes the setting with the given ID.
     * @param id The setting ID
     */
    static void removeKey(const QString &id);

    /**
     * @brief Sets the value of the given setting ID back to its default value.
     *
     * This does nothing and returns an empty QVariant if the setting ID is not registered.
     * @param id The setting ID
     * @return The value that was set
     */
    static QVariant restoreDefaultValue(const QString &id);

    /**
     * @brief Checks if the given key exists.
     * @param key The key to check
     * @return Whether the key exists or not
     */
    static bool keyExists(const QString &key);

    /**
     * @brief Returns a list of all keys that exist.
     * @return The list of all keys
     */
    static QStringList keys();

    /**
     * @brief Returns a list of all groups that exist.
     * @return The list of all groups
     */
    static QStringList groups();

    /**
     * @brief Returns a list of all sub-keys inside the given group.
     * @param groupID The group ID
     * @return The list of sub-keys
     */
    static QStringList keysInGroup(const QString &groupID);

    /**
     * @brief Returns a list of all sub-groups inside the given group.
     * @param groupID The group ID
     * @return The list of sub-groups
     */
    static QStringList groupSubGroups(const QString &groupID);

    /**
     * @brief Returns a set of all setting keys that require a application restart.
     * @return The set of restart requiring settings
     */
    static QSet<QString> restartRequiredSettings();

    /**
     * @brief Checks if the setting specified by the given ID requires a restart.
     *
     * Returns false if the ID is not found.
     * @param id The setting ID
     * @return Whether the settings requires a restart or not
     */
    static bool settingRequiresRestart(const QString &id);

    /**
     * @brief Registers a new SettingsItem to the global configuration
     * @param item The item to register
     */
    static void registerNewSettingsItem(const SettingsItem &item);

    /**
     * @brief Checks if an application restart is required to apply the changes.
     *
     * This returns true if at least one setting that requires a restart was changed.
     * @return Whether a restart is required or not
     */
    static bool restartRequired();

    /**
     * @brief Returns a list of all changed settings that require a restart.
     *
     * This list is sorted alphabetically.
     * @return THe list of settings
     */
    static QStringList modifiedRestartRequiredSettings();

    /**
     * @brief Processes an item inside the SettingsManager.
     *
     * If the item isn't registered yet, this function does nothing.
     *
     * Otherwise, this function registers the item to the _restartRequiredSettings set if needed and
     * sets it to the default value if it hasn't been set.
     * @param id The id of the item
     */
    static void processItem(const QString &id);

    static void callOnChange(QObject *owner, FilterFunction filter, CallbackFunction callback);

protected:
    /**
     * @brief Reads a setting silent (without logging).
     * @param id The setting ID to read
     * @return The value of the setting
     */
    static QVariant readSilent(const QString &id);

signals:
    /**
     * @brief This signals is emited whenever a new value was added to the settings.
     * @param id The setting ID/key
     * @param value The value that was added
     */
    void valueAdded(const QString &id, const QVariant &value);

    /**
     * @brief This signals is emited whenever an existing value was changed.
     * @param id The setting ID/key
     * @param value The value that was set
     */
    void valueChanged(const QString &id, const QVariant &value);

    /**
     * @brief This signals is emited whenever an existing value was removed
     * @param id The setting ID/key
     */
    void keyRemoved(const QString &id);

private:
    struct CallbackEntry {
        bool ownerDependet;
        QPointer<QObject> owner;
        FilterFunction filter;
        CallbackFunction callback;
    };

    /// QSettings object for the actual reading/writing
    static inline QSettings _settings = QSettings("ScheduleMaster", "ScheduleMaster");

    /// Set of all settings IDs that require an application restart when they change
    static inline QSet<QString> _restartRequiredSettings;

    /// Set of all settings IDs that were changed and require an application restart
    static inline QSet<QString> _modifiedRestartRequiredSettings;

    static inline QList<CallbackEntry> _callbacks;
};

#endif // SETTINGSMANAGER_H
