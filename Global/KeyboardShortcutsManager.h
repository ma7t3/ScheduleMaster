#ifndef KEYBOARDSHORTCUTSMANAGER_H
#define KEYBOARDSHORTCUTSMANAGER_H

#include <QObject>
#include <QKeySequence>

#include "GlobalConfigManager.h"
#include "Global/SettingsManager.h"
#include "Global/Global.h"

class KeyboardShortcut : public GlobalConfigItem {
public:
    KeyboardShortcut(const QJsonObject &jsonObject = QJsonObject());
    KeyboardShortcut(const QString &id);

    QString description, icon;
    QKeySequence defaultKeySequence;
};

class KeyboardShortcutsManager : public GlobalConfigManager<KeyboardShortcutsManager, KeyboardShortcut> {
    friend class GlobalConfigManager<KeyboardShortcutsManager, KeyboardShortcut>;
    Q_OBJECT
protected:
    explicit KeyboardShortcutsManager(QObject *parent);

public:
    /**
     * @brief Checks if the given QKeySequence is the shortcut's default key sequence
     * @param id The KeyboardShortcut's ID
     * @param sequence The QKeySequence to check for
     * @return Whether sequence is id's default key sequence or not
     */
    static bool isDefault(const QString &id, const QKeySequence &sequence);
};

#endif // KEYBOARDSHORTCUTSMANAGER_H
