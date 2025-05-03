#ifndef KEYBOARDSHORTCUTMANAGER_H
#define KEYBOARDSHORTCUTMANAGER_H

#include <QObject>
#include <QKeySequence>

#include "GlobalConfigManager.h"
#include "Global/SettingsManager.h"
#include "Global/Global.h"

struct KeyboardShortcutConfig : public GlobalConfigItem {
public:
    KeyboardShortcutConfig(const QJsonObject &jsonObject = QJsonObject());
    KeyboardShortcutConfig(const QString &id);

    QString description, icon;
    QKeySequence defaultKeySequence;
};

class KeyboardShortcutManager : public GlobalConfigManager<KeyboardShortcutManager, KeyboardShortcutConfig> {
    friend class GlobalConfigManager<KeyboardShortcutManager, KeyboardShortcutConfig>;
    Q_OBJECT
protected:
    explicit KeyboardShortcutManager(QObject *parent);

public:
    /**
     * @brief Checks if the given QKeySequence is the shortcut's default key sequence
     * @param id The KeyboardShortcut's ID
     * @param sequence The QKeySequence to check for
     * @return Whether sequence is id's default key sequence or not
     */
    static bool isDefault(const QString &id, const QKeySequence &sequence);

    static QKeySequence keyboardShortcut(const QString &keyboardShortcutID);
    static void setKeyboardShortcut(const QString &keyboardShortcutID, const QKeySequence shortcut);

    static void importJson(const QJsonArray &jsonArray);
    static QJsonArray exportJson();

signals:
    void keyboardShortcutChanged(const QString &keyboardShortcutID, const QKeySequence shortcut);
};

#endif // KEYBOARDSHORTCUTMANAGER_H
