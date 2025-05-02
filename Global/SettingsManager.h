#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include "GlobalConfigManager.h"

#include "Global/VariantConverter.h"

class SettingsItem : public GlobalConfigItem {
public:
    SettingsItem(const QJsonObject &jsonObject = QJsonObject());
    SettingsItem(const QString &id);

    QString description;
    QVariant defaultValue;
    QMetaType::Type type = QMetaType::Void, groupContentType = QMetaType::Void;
    bool isGroup = false, requiresRestart = false;
};

class SettingsManager : public GlobalConfigManager<SettingsManager, SettingsItem> {
    friend class GlobalConfigManager<SettingsManager, SettingsItem>;
    Q_OBJECT
protected:
    explicit SettingsManager(QObject *parent);

public:
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

private:
    static inline QSet<QString> _restartRequiredSettings;
};

#endif // SETTINGSMANAGER_H
