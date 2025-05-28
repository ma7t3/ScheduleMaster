#include "KeyboardShortcutManager.h"

#include "Global/Global.h"
#include "Global/SettingsManager.h"

KeyboardShortcutConfig::KeyboardShortcutConfig(const QJsonObject &jsonObject, const int &index) : GlobalConfigItem(jsonObject, index) {
    text        = jsonObject.value("text").toString(id());
    tooltip     = jsonObject.value("tooltip").toString();
    description = jsonObject.value("description").toString(text);
    icon        = jsonObject.value("icon").toString();

    QJsonValue defaultValue = jsonObject.value("defaultKeyboardShortcut");
    QStringList defaultValues;

    if(defaultValue.isArray())
        defaultValues = Global::jsonArrayToStringList(defaultValue.toArray());
    else
        defaultValues = {defaultValue.toString()};

    for(const QString & value : std::as_const(defaultValues)) {
        if(value.startsWith("standard:")) {
            QString idPart = value.mid(QString("standard:").length());
            bool ok;
            int standardID = idPart.toInt(&ok);
            if (ok) {
                QKeySequence sequence = QKeySequence(static_cast<QKeySequence::StandardKey>(standardID));
                if(!sequence.toString(QKeySequence::PortableText).isEmpty()) {
                    defaultKeySequence = sequence;
                    return;
                } else {
                    defaultKeySequence = QKeySequence();
                }
            }
        }

        QKeySequence sequence(value);
        if (!sequence.toString(QKeySequence::PortableText).isEmpty()) {
            defaultKeySequence = sequence;
            return;
        } else
            defaultKeySequence = QKeySequence();
    }
}

KeyboardShortcutConfig::KeyboardShortcutConfig(const QString &id, const int &index) : GlobalConfigItem(id, index) {}

KeyboardShortcutManager::KeyboardShortcutManager(QObject *parent) :
    GlobalConfigManager(parent) {
    loadItems("Actions");
    const QList<KeyboardShortcutConfig> itemList = items();
    for(const KeyboardShortcutConfig &shortcut : itemList) {
        SettingsItem item("keyboardShortcuts/" + shortcut.id());
        item.type         = QMetaType::QKeySequence;
        item.description  = shortcut.description;
        item.defaultValue = shortcut.defaultKeySequence;
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

bool KeyboardShortcutManager::isDefault(const QString &id, const QKeySequence &sequence) {
    return itemExists(id) ? item(id).defaultKeySequence == sequence : false;
}

QKeySequence KeyboardShortcutManager::keyboardShortcut(const QString &keyboardShortcutID) {
    const QString fullID = "keyboardShortcuts/" + keyboardShortcutID;
    if(SettingsManager::keyExists(fullID))
        return QKeySequence(SettingsManager::value(fullID).toString());

    if(!itemExists(keyboardShortcutID))
        return QKeySequence();

    const QKeySequence sequence = item(keyboardShortcutID).defaultKeySequence;
    setKeyboardShortcut(keyboardShortcutID, sequence);
    return sequence;
}

void KeyboardShortcutManager::setKeyboardShortcut(const QString &keyboardShortcutID, const QKeySequence shortcut) {
    SettingsManager::setValue("keyboardShortcuts/" + keyboardShortcutID, shortcut.toString(QKeySequence::PortableText));
    //emit instance()->keyboardShortcutChanged(keyboardShortcutID, shortcut);
}

void KeyboardShortcutManager::importJson(const QJsonArray &jsonArray) {
    for(const QJsonValue &value : jsonArray) {
        const QJsonObject jObj = value.toObject();
        const QString id          = jObj.value("id").toString();
        const QString keySequence = jObj.value("keySequence").toString();
        setKeyboardShortcut(id, keySequence);
    }
}

QJsonArray KeyboardShortcutManager::exportJson() {
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
