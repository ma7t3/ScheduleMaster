#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>
#include <QKeySequence>

#include "GlobalConfigManager.h"

struct ActionConfig : public GlobalConfigItem {
    ActionConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    ActionConfig(const QString &id, const int &index = 0);

    bool canHaveShortcut;
    QString text, tooltip, description, icon;
    QStringList breadcrumb;
    QKeySequence defaultKeyboardShortcut;
};

class ActionManager : public GlobalConfigManager<ActionManager, ActionConfig> {
    friend class GlobalConfigManager<ActionManager, ActionConfig>;
    Q_OBJECT

private:
    explicit ActionManager(QObject *parent);

public:
    static ActionManager *instance();
    static void init();

    static bool shortcutIsDefault(const QString &id, const QKeySequence &sequence);

    static QKeySequence keyboardShortcut(const QString &actionID);
    static void setKeyboardShortcut(const QString &actionID, const QKeySequence shortcut);

    static void importKeyboardShortcuts(const QJsonArray &jsonArray);
    static QJsonArray exportKeyboardShortcuts();

    ActionManager(ActionManager &other) = delete;
    void operator=(const ActionManager &) = delete;

signals:
    void keyboardShortcutChanged(const QString &keyboardShortcutID, const QKeySequence shortcut);
};

#endif // ACTIONMANAGER_H
