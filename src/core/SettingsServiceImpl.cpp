#include "SettingsServiceImpl.h"

#include "Global/VariantConverter.h"

namespace ScheduleMaster::Core {

SettingsServiceImpl::SettingsServiceImpl(QObject *parent) : GlobalConfigServiceCRTP(parent, "Settings"), _settings("ScheduleMaster", "ScheduleMaster") {
    const QList<SettingsItem> list = repository()->items();
    for(const SettingsItem &item : list)
        processItem(item.id());
}

void SettingsServiceImpl::init() {}

QVariant SettingsServiceImpl::value(const QString &id) const {
    QVariant value = readSilent(id);

    qDebug().noquote() << "Read setting: " << id << " << " << value;

    return value;
}

QVariant SettingsServiceImpl::setValue(const QString &id, const QVariant &value) {
    bool exists = repository()->itemExists(id);

    QVariant convVal;

    if(exists)
        convVal = VariantConverter::convert(value, static_cast<QMetaType::Type>(repository()->item(id).type));
    else
        convVal = value;

    bool keyExists = this->keyExists(id);
    if(keyExists && readSilent(id) == convVal)
        return convVal;

    bool restartRequired = settingRequiresRestart(id);
    if(restartRequired)
        _modifiedRestartRequiredSettings << id;

    qDebug().noquote() << "Save setting: " << id << " = " << convVal << (restartRequired ? " (requires restart)" : "");

    _settings.setValue(id, convVal);

    if(keyExists)
        emit valueChanged(id, value);
    else
        emit valueAdded(id, value);

    return convVal;
}

QVariant SettingsServiceImpl::restoreDefaultValue(const QString &id) {
    if(!repository()->itemExists(id)) {
        qWarning().noquote() << "Cannot reset setting to default: " << id << " - setting is not registered to ScheduleMaster!";
        return QVariant();
    }

    qDebug().noquote() << "Reset setting to default: " << id;
    return setValue(id, repository()->item(id).defaultValue);
}

void SettingsServiceImpl::removeKey(const QString &id) {
    bool restartRequired = settingRequiresRestart(id);

    if(restartRequired)
        _modifiedRestartRequiredSettings << id;

    qDebug().noquote() << "Remove setting: " << id << (restartRequired ? " (requires restart)" : "");

    _settings.remove(id);
    emit keyRemoved(id);
}

bool SettingsServiceImpl::keyExists(const QString &key) const {
    return _settings.contains(key);
}

QStringList SettingsServiceImpl::keys() const {
    return _settings.childKeys();
}

QStringList SettingsServiceImpl::groups() const {
    return _settings.childGroups();
}

QStringList SettingsServiceImpl::keysInGroup(const QString &groupID) const {
    auto *self = const_cast<SettingsServiceImpl *>(this);
    self->_settings.beginGroup(groupID);
    QStringList keys = _settings.childKeys();
    self->_settings.endGroup();
    return keys;
}

QStringList SettingsServiceImpl::groupSubGroups(const QString &groupID) const {
    auto *self = const_cast<SettingsServiceImpl *>(this);
    self->_settings.beginGroup(groupID);
    QStringList groups = _settings.childGroups();
    self->_settings.endGroup();
    return groups;
}

QSet<QString> SettingsServiceImpl::restartRequiredSettings() const {
    return _restartRequiredSettings;
}

bool SettingsServiceImpl::settingRequiresRestart(const QString &id) const {
    return _restartRequiredSettings.contains(id);
}

bool SettingsServiceImpl::restartRequired() const {
    return !_modifiedRestartRequiredSettings.isEmpty();
}

QStringList SettingsServiceImpl::modifiedRestartRequiredSettings() const {
    QStringList list = _modifiedRestartRequiredSettings.values();
    list.sort();
    return list;
}

bool SettingsServiceImpl::settingExists(const QString &id) const {
    return repository()->itemExists(id);
}

SettingsItem SettingsServiceImpl::settingMetaData(const QString &id) const {
    return repository()->item(id);
}

bool SettingsServiceImpl::registerSetting(const SettingsItem &item) {
    if(item.id().isEmpty())
        return false;

    if(!repository()->addItem(item))
        return false;

    processItem(item.id());

    qInfo().noquote() << "Registered new settings item: " + item.id();
    return true;
}

void SettingsServiceImpl::processItem(const QString &id) {
    const SettingsItem item = repository()->item(id);

    if(!_settings.contains(item.id()) && !item.isGroup)
        _settings.setValue(item.id(), item.defaultValue);

    if(item.requiresRestart)
        _restartRequiredSettings << item.id();
}

QVariant SettingsServiceImpl::readSilent(const QString &id) const {
    QVariant value;
    if(repository()->itemExists(id)) {
        SettingsItem itm = repository()->item(id);
        QVariant rawValue = _settings.value(id, itm.defaultValue);
        value = VariantConverter::convert(rawValue, itm.type);
        if(!keyExists(id) && !itm.isGroup) {
            auto *self = const_cast<SettingsServiceImpl *>(this);
            self->setValue(id, value); // init with default value
        }

        return value;
    } else {
        return _settings.value(id);
    }
}

}
