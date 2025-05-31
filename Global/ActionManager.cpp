#include "ActionManager.h"

#include "Global/SettingsManager.h"

#include "Global/Global.h"

ActionConfig::ActionConfig(const QJsonObject &jsonObject, const int &index) : GlobalConfigItem(jsonObject, index) {
    text        = jsonObject.value("text").toString(id());
    tooltip     = jsonObject.value("tooltip").toString();
    description = jsonObject.value("description").toString(text);
    icon        = jsonObject.value("icon").toString();
    breadcrumb  = jsonObject.value("breadcrumb").toString().split(" > ");

    canHaveShortcut = jsonObject.contains("defaultKeyboardShortcut");
    defaultKeyboardShortcut = Global::parseKeyboardShortcutConfigString(jsonObject.value("defaultKeyboardShortcut"));
}

ActionConfig::ActionConfig(const QString &id, const int &index) : GlobalConfigItem(id, index) {}

ActionManager::ActionManager(QObject *parent) : GlobalConfigManager(parent) {
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

    SettingsManager::callOnChange(this,
    [](const QString &id){
        return id.startsWith("keyboardShortcuts/");
    },
    [](const QString &settingID, const QVariant &value) {
        QString shortcutID = settingID;
        shortcutID.remove("keyboardShortcuts/");
        emit instance()->keyboardShortcutChanged(shortcutID, QKeySequence(value.toString()));
        qDebug() << "Callback Recived: Shortcut changed!" << shortcutID << value;
    });
}

ActionManager *ActionManager::instance() {
    static ActionManager instance(nullptr);
    return &instance;
}

void ActionManager::init() {
    instance();
}

bool ActionManager::shortcutIsDefault(const QString &id, const QKeySequence &sequence) {
    return itemExists(id) ? item(id).defaultKeyboardShortcut == sequence : false;
}

QKeySequence ActionManager::keyboardShortcut(const QString &actionID) {
    if(!itemExists(actionID) || !item(actionID).canHaveShortcut)
        return QKeySequence();

    const QString fullID = "keyboardShortcuts/" + actionID;
    if(SettingsManager::keyExists(fullID))
        return QKeySequence(SettingsManager::value(fullID).toString());

    const QKeySequence sequence = item(actionID).defaultKeyboardShortcut;
    setKeyboardShortcut(actionID, sequence);
    return sequence;
}

void ActionManager::setKeyboardShortcut(const QString &actionID, const QKeySequence shortcut) {
    SettingsManager::setValue("keyboardShortcuts/" + actionID, shortcut.toString(QKeySequence::PortableText));
}

void ActionManager::importKeyboardShortcuts(const QJsonArray &jsonArray) {
    for(const QJsonValue &value : jsonArray) {
        const QJsonObject jObj = value.toObject();
        const QString id          = jObj.value("id").toString();
        const QString keySequence = jObj.value("keySequence").toString();
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
        settingsItem.type = QMetaType::QKeySequence;
        settingsItem.description = actionConfig.description;
        settingsItem.defaultValue = actionConfig.defaultKeyboardShortcut;
        SettingsManager::registerNewSettingsItem(settingsItem);
        return true;
    }
    return false;
}
