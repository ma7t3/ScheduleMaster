#include "ActionManager.h"

#include "Global/SettingsManager.h"

#include "Global/Global.h"

ActionConfig::ActionConfig(const QJsonObject &jsonObject, const int &index) : GlobalConfigItem(jsonObject, index) {
    text        = jsonObject.value("text").toString(id());
    tooltip     = jsonObject.value("tooltip").toString();
    description = jsonObject.value("description").toString(text);
    icon        = jsonObject.value("icon").toString();
    breadcrumb  = jsonObject.value("breadcrumb").toString().split(" > ");

    if(breadcrumb.join("").isEmpty())
        breadcrumb.clear();

    canHaveShortcut         = jsonObject.contains("defaultKeyboardShortcut");
    defaultKeyboardShortcut = Global::parseKeyboardShortcutConfigString(
        jsonObject.value("defaultKeyboardShortcut"));
}

void ActionManager::init() {
    GlobalConfigManager::init();

    loadItems("Actions");
    const QList<ActionConfig> itemList = items();
    for(const ActionConfig &shortcut : itemList) {
        if(!shortcut.canHaveShortcut)
            continue;

        SettingsItem item("keyboardShortcuts/" + shortcut.id());
        item.type         = QMetaType::QKeySequence;
        item.description  = shortcut.description;
        item.defaultValue = shortcut.defaultKeyboardShortcut;
        SettingsManager::registerNewSettingsItem(item);
    }

    connect(SettingsManager::instance(), &SettingsManager::valueChanged, instance(), [](const QString &settingID, const QVariant &value){
        if(!settingID.startsWith("keyboardShortcuts/"))
            return;

        QString shortcutID = settingID;
        const QKeySequence sequence = QKeySequence(value.toString());
        shortcutID.remove("keyboardShortcuts/");
        _keyboardShortcutsCache.insert(shortcutID, sequence);
        emit instance()->keyboardShortcutChanged(shortcutID, sequence);
    });
}

bool ActionManager::shortcutIsDefault(const QString &id, const QKeySequence &sequence) {
    return itemExists(id) ? item(id).defaultKeyboardShortcut == sequence : false;
}

QKeySequence ActionManager::keyboardShortcut(const QString &actionID) {
    if(!itemExists(actionID) || !item(actionID).canHaveShortcut)
        return QKeySequence();

    if(_keyboardShortcutsCache.contains(actionID))
        return _keyboardShortcutsCache.value(actionID);

    const QString fullID = "keyboardShortcuts/" + actionID;

    if(SettingsManager::keyExists(fullID)) {
        const QKeySequence sequence = QKeySequence(SettingsManager::value(fullID).toString());
        _keyboardShortcutsCache.insert(actionID, sequence);
        return sequence;
    }

    const QKeySequence sequence = item(actionID).defaultKeyboardShortcut;
    setKeyboardShortcut(actionID, sequence);
    return sequence;
}

void ActionManager::setKeyboardShortcut(const QString &actionID, const QKeySequence shortcut) {
    SettingsManager::setValue("keyboardShortcuts/" + actionID,
                              shortcut.toString(QKeySequence::PortableText));

    _keyboardShortcutsCache.insert(actionID, shortcut);
}

void ActionManager::importKeyboardShortcuts(const QJsonArray &jsonArray) {
    for(const QJsonValue &value : jsonArray) {
        const QJsonObject jObj        = value.toObject();
        const QString     id          = jObj.value("id").toString();
        const QString     keySequence = jObj.value("keySequence").toString();
        setKeyboardShortcut(id, keySequence);
    }
}

QJsonArray ActionManager::exportKeyboardShortcuts() {
    QJsonArray array;
    const QStringList keys = SettingsManager::keysInGroup("keyboardShortcuts");
    for(const QString &id : keys) {
        QJsonObject obj;
        obj["id"] = id;
        obj["keySequence"] = keyboardShortcut(id).toString();
        array << obj;
    }
    return array;
}

bool ActionManager::addItem(const ActionConfig &actionConfig) {
    if(GlobalConfigManager::addItem(actionConfig) && actionConfig.canHaveShortcut) {
        SettingsItem settingsItem("keyboardShortcuts/" + actionConfig.id());
        settingsItem.type         = QMetaType::QKeySequence;
        settingsItem.description  = actionConfig.description;
        settingsItem.defaultValue = actionConfig.defaultKeyboardShortcut;
        SettingsManager::registerNewSettingsItem(settingsItem);
        return true;
    }
    return false;
}
