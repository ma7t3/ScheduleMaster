#include "KeyboardShortcutsManager.h"

KeyboardShortcut::KeyboardShortcut(const QJsonObject &jsonObject) : GlobalConfigItem(jsonObject) {
    description = jsonObject.value("description").toString(id());
    icon        = jsonObject.value("icon").toString();

    QJsonValue defaultValue = jsonObject.value("default");
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

KeyboardShortcut::KeyboardShortcut(const QString &id) : GlobalConfigItem(id) {}

KeyboardShortcutsManager::KeyboardShortcutsManager(QObject *parent) :
    GlobalConfigManager(parent) {
    loadItems("KeyboardShortcuts");
    for(KeyboardShortcut shortcut : items()) {
        SettingsItem item("keyboardShortcuts/" + shortcut.id());
        item.type         = QMetaType::QKeySequence;
        item.description  = shortcut.description;
        item.defaultValue = shortcut.defaultKeySequence;
        SettingsManager::registerNewSettingsItem(item);
    }
}

bool KeyboardShortcutsManager::isDefault(const QString &id, const QKeySequence &sequence) {
    return itemExists(id) ? item(id).defaultKeySequence == sequence : false;
}
