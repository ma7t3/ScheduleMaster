#ifndef ISETTINGSSERVICE_H
#define ISETTINGSSERVICE_H

#include "ScheduleMaster_global.h"
#include "GlobalConfigItem.h"

namespace ScheduleMaster {

class SettingsItem : public GlobalConfigItem {
public:
    SettingsItem(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    SettingsItem(const QString &id, const int &index = 0);

    QString description;
    QVariant defaultValue;
    QMetaType::Type type = QMetaType::Void, groupContentType = QMetaType::Void;
    bool isGroup = false, requiresRestart = false, dontTouchWarning = false;
};

class SCHEDULEMASTERINTERFACE_EXPORT ISettingsService {
public:
    virtual ~ISettingsService() = default;

    virtual QVariant value(const QString &id) const = 0;
    virtual QVariant setValue(const QString &id, const QVariant &value) = 0;
    virtual QVariant restoreDefaultValue(const QString &id) = 0;
    virtual void removeKey(const QString &id) = 0;

    virtual bool keyExists(const QString &key) const = 0;
    virtual QStringList keys() const = 0;
    virtual QStringList groups() const = 0;
    virtual QStringList keysInGroup(const QString &groupID) const = 0;
    virtual QStringList groupSubGroups(const QString &groupID) const = 0;

    virtual QSet<QString> restartRequiredSettings() const = 0;
    virtual bool settingRequiresRestart(const QString &id) const = 0;
    virtual bool restartRequired() const = 0;
    virtual QStringList modifiedRestartRequiredSettings() const = 0;

    virtual bool settingExists(const QString &id) const = 0;
    virtual SettingsItem settingMetaData(const QString &id) const = 0;
    virtual bool registerSetting(const SettingsItem &item) = 0;
};

}

#endif // ISETTINGSSERVICE_H
