#ifndef SETTINGSSERVICEIMPL_H
#define SETTINGSSERVICEIMPL_H

#include <QObject>
#include <QSettings>

#include "GlobalConfigRepository.h"
#include "GlobalConfigService.h"

#include "src/api/ISettingsService.h"

namespace ScheduleMaster::Core {

using SettingsRepository = GlobalConfigRepositoryCRTP<SettingsItem>;

class SettingsServiceImpl :
    public GlobalConfigServiceCRTP<SettingsRepository, SettingsServiceImpl>,
    public ISettingsService {
    Q_OBJECT

public:
    explicit SettingsServiceImpl(QObject *parent = nullptr);

    virtual QVariant value(const QString &id) const override;
    virtual QVariant setValue(const QString &id, const QVariant &value) override;
    virtual QVariant restoreDefaultValue(const QString &id) override;
    virtual void removeKey(const QString &id) override;

    virtual bool keyExists(const QString &key) const override;
    virtual QStringList keys() const override;
    virtual QStringList groups() const override;
    virtual QStringList keysInGroup(const QString &groupID) const override;
    virtual QStringList groupSubGroups(const QString &groupID) const override;

    virtual QSet<QString> restartRequiredSettings() const override;
    virtual bool settingRequiresRestart(const QString &id) const override;
    virtual bool restartRequired() const override;
    virtual QStringList modifiedRestartRequiredSettings() const override;

    virtual bool settingExists(const QString &id) const override;
    virtual SettingsItem settingMetaData(const QString &id) const override;
    virtual bool registerSetting(const SettingsItem &item) override;

protected:
    QVariant readSilent(const QString &id) const;

protected slots:
    void processItem(const QString &id);

signals:
    void valueAdded(const QString &id, const QVariant &value);
    void valueChanged(const QString &id, const QVariant &value);
    void keyRemoved(const QString &id);

private:
    QSettings _settings;
    QSet<QString> _restartRequiredSettings;
    QSet<QString> _modifiedRestartRequiredSettings;
};

}

#endif // SETTINGSSERVICEIMPL_H
