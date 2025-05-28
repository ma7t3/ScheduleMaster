#ifndef ACTIONCONTROLLER_H
#define ACTIONCONTROLLER_H

#include <QObject>
#include <QMenu>
#include <QCommandLinkButton>
#include <QToolButton>

#include "ActionManager.h"
#include "IconController.h"

class ActionController : public QObject {
    Q_OBJECT
private:
    explicit ActionController(QObject *parent);

public:
    static void init();
    static ActionController *instance();

    ActionController& operator=(const ActionController&) = delete;
    ActionController(const ActionController&) = delete;

    static QAction *createAction(const QString &actionID, QObject *parent = nullptr);
    static QMenu *createMenu(const QString &actionID, QObject *parent = nullptr);
    static QPushButton *createPushButton(const QString &actionID, QWidget *parent = nullptr);
    static QToolButton *createToolButton(const QString &actionID, QWidget *parent = nullptr);
    static QCommandLinkButton *createCommandLinkButton(const QString &actionID, QWidget *parent = nullptr);

    template<typename T>
    static T *add(T *widget, const QString &actionID) {
    if(!ActionManager::itemExists(actionID)) {
        qWarning() << "cannot add actoin for actionID" << actionID << "- action does not exist in ActionManager.";
        return widget;
    }

    ActionConfig actionConfig = ActionManager::item(actionID);

    if constexpr(std::is_base_of<QAbstractButton, T>::value) {
        QAbstractButton *button = static_cast<QAbstractButton *>(widget);
        _buttons.insert(button, actionID);
        connect(button, &QObject::destroyed, instance(), [button](){ _buttons.remove(button); });

        button->setText(actionConfig.text);
        button->setToolTip(actionConfig.tooltip);
        button->setIcon(IconController::icon(actionConfig.icon));

        QKeySequence shortcut = ActionManager::keyboardShortcut(actionID);
        button->setShortcut(shortcut);

    } else if constexpr(std::is_base_of<QAction, T>::value) {
        QAction *action = static_cast<QAction *>(widget);
        _actions.insert(action, actionID);
        connect(action, &QObject::destroyed, instance(), [action](){ _actions.remove(action); });

        action->setText(actionConfig.text);
        action->setToolTip(actionConfig.tooltip);
        action->setIcon(IconController::icon(actionConfig.icon));

        QKeySequence shortcut = ActionManager::keyboardShortcut(actionID);
        action->setShortcut(shortcut);

    } else if constexpr(std::is_base_of<QMenu, T>::value) {
        QMenu *menu = static_cast<QMenu *>(widget);
        _menus.insert(menu, actionID);
        connect(menu, &QObject::destroyed, instance(), [menu](){ _menus.remove(menu); });

        menu->setTitle(actionConfig.text);
        menu->setToolTip(actionConfig.tooltip);
        menu->setIcon(IconController::icon(actionConfig.icon));
    } else {
        qWarning() << "Cannot add action to ActionController. Unsupported type:" << typeid(T).name();
    }

        return widget;
    }

    template <typename T>
    static T *remove(T *widget);

protected slots:
    void onIconSetChanged();
    void onActionShortcutChanged(const QString &keyboardShortcutID, const QKeySequence shortcut);

signals:

private:
    static inline QHash<QMenu *, QString> _menus;
    static inline QHash<QAction *, QString> _actions;
    static inline QHash<QAbstractButton *, QString> _buttons;
};

#endif // ACTIONCONTROLLER_H
