#include "SettingsManager.h"

#include "Global/VariantConverter.h"

SettingsItem::SettingsItem(const QString &id, const int &index) : GlobalConfigItem(id, index) {}

SettingsItem::SettingsItem(const QJsonObject &jsonObject, const int &index) : GlobalConfigItem(jsonObject, index) {
    description = jsonObject.value("description").toString();

    QString typeStr = jsonObject.value("type").toString();
    if (typeStr == "group")
        isGroup = true;

    typeStr = isGroup ? jsonObject.value("content_type").toString() : typeStr;

    type = static_cast<QMetaType::Type>(QMetaType::fromName(typeStr.toUtf8()).id());

    if (isGroup) {
        groupContentType = type;
        type = QMetaType::Void;
    }

    if (!isGroup)
        if(jsonObject.contains("default"))
            defaultValue = VariantConverter::convertFromJson(jsonObject.value("default"), type);

    requiresRestart = jsonObject.value("requireRestart").toBool(false);
}

SettingsManager::SettingsManager(QObject *parent) :
    GlobalConfigManager(parent) {
    loadItems("Settings");
    const QList<SettingsItem> list = items();
    for(const SettingsItem &item : list)
        processItem(item.id());
}

QVariant SettingsManager::value(const QString &id) {
    QVariant value = readSilent(id);

    //Logger::setPrefix(staticMetaObject.className());
    qDebug().noquote() << "Read setting: " << id << " << " << value;
    //Logger::resetPrefix();

    return value;
}

QVariant SettingsManager::setValue(const QString &id, const QVariant &value) {
    bool exists = itemExists(id);

    QVariant convVal;

    if(exists)
        convVal = VariantConverter::convert(value, static_cast<QMetaType::Type>(item(id).type));
    else
        convVal = value;

    bool keyExists = SettingsManager::keyExists(id);
    if(keyExists && readSilent(id) == convVal)
        return convVal;

    bool restartRequired = settingRequiresRestart(id);
    if(restartRequired)
        _modifiedRestartRequiredSettings << id;

    //Logger::setPrefix(staticMetaObject.className());
    qDebug().noquote() << "Save setting: " << id << " = " << convVal << (restartRequired ? " (requires restart)" : "");
    //Logger::resetPrefix();

    _settings.setValue(id, convVal);

    if(keyExists)
        emit instance()->valueChanged(id, value);
    else
        emit instance()->valueAdded(id, value);

    for(const CallbackEntry &callback : std::as_const(_callbacks)) {
        if(callback.ownerDependet && !callback.owner)
            continue;

        if(callback.filter(id))
            callback.callback(id, convVal);
    }

    return convVal;
}

void SettingsManager::removeKey(const QString &id) {
    bool restartRequired = settingRequiresRestart(id);

    if(restartRequired)
        _modifiedRestartRequiredSettings << id;

    //Logger::setPrefix(staticMetaObject.className());
    qDebug().noquote() << "Remove setting: " << id << (restartRequired ? " (requires restart)" : "");
    //Logger::resetPrefix();

    _settings.remove(id);
    emit instance()->keyRemoved(id);
}

QVariant SettingsManager::restoreDefaultValue(const QString &id) {
    if(!itemExists(id)) {
        qWarning().noquote() << "Cannot reset setting to default: " << id << " - setting is not registered to ScheduleMaster!";
        return QVariant();
    }

    qDebug().noquote() << "Reset setting to default: " << id;
    return setValue(id, item(id).defaultValue);
}

bool SettingsManager::keyExists(const QString &key) {
    return _settings.contains(key);
}

QStringList SettingsManager::keys() {
    return _settings.childKeys();
}

QStringList SettingsManager::groups() {
    return _settings.childGroups();
}

QStringList SettingsManager::keysInGroup(const QString &groupID) {
    _settings.beginGroup(groupID);
    QStringList keys = _settings.childKeys();
    _settings.endGroup();
    return keys;
}

QStringList SettingsManager::groupSubGroups(const QString &groupID) {
    _settings.beginGroup(groupID);
    QStringList groups = _settings.childGroups();
    _settings.endGroup();
    return groups;
}

QSet<QString> SettingsManager::restartRequiredSettings() {
    return _restartRequiredSettings;
}

bool SettingsManager::settingRequiresRestart(const QString &id) {
    return _restartRequiredSettings.contains(id);
}

void SettingsManager::registerNewSettingsItem(const SettingsItem &item) {
    if(item.id().isEmpty())
        return;

    addItem(item);
    processItem(item.id());

    qInfo().noquote() << "Registered new settings item: " + item.id();
}

bool SettingsManager::restartRequired() {
    return !_modifiedRestartRequiredSettings.isEmpty();
}

QStringList SettingsManager::modifiedRestartRequiredSettings() {
    QStringList list = _modifiedRestartRequiredSettings.values();
    list.sort();
    return list;
}

void SettingsManager::processItem(const QString &id) {
    const SettingsItem item = GlobalConfigManager::item(id);

    if(!_settings.contains(item.id()) && !item.isGroup)
        _settings.setValue(item.id(), item.defaultValue);

    if(item.requiresRestart)
        _restartRequiredSettings << item.id();
}

void SettingsManager::callOnChange(QObject *owner, FilterFunction filter, CallbackFunction callback) {
    _callbacks << CallbackEntry{owner != nullptr, QPointer<QObject>(owner), filter, callback};
}

QVariant SettingsManager::readSilent(const QString &id) {
    QVariant value;
    if(itemExists(id)) {
        SettingsItem itm = item(id);
        QVariant rawValue = _settings.value(id, itm.defaultValue);
        value = VariantConverter::convert(rawValue, itm.type);
        if(!keyExists(id) && !itm.isGroup)
            setValue(id, value); // init with default value

        return value;
    } else {
        return _settings.value(id);
    }
}
