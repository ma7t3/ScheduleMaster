#ifndef ACTIONCONTROLLER_H
#define ACTIONCONTROLLER_H

#include <QObject>
#include <QMenu>
#include <QCommandLinkButton>
#include <QToolButton>

#include "ActionManager.h"
#include "IconController.h"

struct GlobalActionWrapper {
    GlobalActionWrapper(QObject *widget = nullptr) : widget(widget) {}

    void execute() {
        QAction *action = qobject_cast<QAction *>(widget);
        QAbstractButton *button = qobject_cast<QAbstractButton *>(widget);

        if(action)
            action->trigger();

        if(button)
            button->click();
    }

    QObject *widget;
};

class ActionController : public QObject {
    Q_OBJECT
private:
    explicit ActionController(QObject *parent);

public:
    enum ActionComponent {
        NoComponents = 0x0,
        IconComponent = 0x1,
        TextComponent = 0x4,
        TooltipComponent = 0x8,
        ShortcutComponent = 0x10,
        AllComponents = IconComponent | TextComponent | TooltipComponent | ShortcutComponent,
        AllExceptIconComponent = AllComponents & ~IconComponent,
        AllExceptTextComponent = AllComponents & ~TextComponent,
        AllExceptTooltipComponent = AllComponents & ~TooltipComponent,
        AllExceptShortcutComponent = AllComponents & ~ShortcutComponent,
    };
    Q_DECLARE_FLAGS(ActionComponents, ActionComponent)

    static void init();
    static ActionController *instance();

    ActionController& operator=(const ActionController&) = delete;
    ActionController(const ActionController&) = delete;

    static QAction *createAction(const QString &actionID, const ActionComponents &components = AllComponents, QObject *parent = nullptr);
    static QMenu *createMenu(const QString &actionID, const ActionComponents &components = AllComponents, QObject *parent = nullptr);
    static QPushButton *createPushButton(const QString &actionID, const ActionComponents &components = AllComponents, QWidget *parent = nullptr);
    static QToolButton *createToolButton(const QString &actionID, const ActionComponents &components = AllComponents, QWidget *parent = nullptr);
    static QCommandLinkButton *createCommandLinkButton(const QString &actionID, const ActionComponents &components = AllComponents, QWidget *parent = nullptr);

    template<typename T>
    static T *addAsGlobalAction(T *widget,
                  const QString &actionID,
                  const ActionComponents &components = AllComponents) {
        add(widget, actionID, components);
        setGlobalAction(actionID, widget);
        return widget;
    }

    template<typename T>
    static T *add(T *widget,
                  const QString &actionID,
                  const ActionComponents &components = AllComponents) {
        if(!ActionManager::itemExists(actionID)) {
            qWarning() << "cannot add action for actionID" << actionID << "- action does not exist in ActionManager.";
            return widget;
        }

        ActionConfig actionConfig = ActionManager::item(actionID);

        if constexpr(std::is_base_of<QAbstractButton, T>::value) {
            QAbstractButton *button = static_cast<QAbstractButton *>(widget);
            _buttons.insert(button, QPair<QString, ActionComponents>(actionID, components));
            connect(button, &QObject::destroyed, instance(), [button](){ _buttons.remove(button); });

            if(components.testFlag(TextComponent))
                button->setText(actionConfig.text);

            if(components.testFlag(TooltipComponent))
                button->setToolTip(actionConfig.tooltip);

            if(components.testFlag(IconComponent))
                button->setIcon(IconController::icon(actionConfig.icon));

            if(components.testFlag(ShortcutComponent)) {
                QKeySequence shortcut = ActionManager::keyboardShortcut(actionID);
                button->setShortcut(shortcut);
            }

        } else if constexpr(std::is_base_of<QAction, T>::value) {
            QAction *action = static_cast<QAction *>(widget);
            _actions.insert(action, QPair<QString, ActionComponents>(actionID, components));
            connect(action, &QObject::destroyed, instance(), [action](){ _actions.remove(action); });

            if(components.testFlag(TextComponent))
                action->setText(actionConfig.text);

            if(components.testFlag(TooltipComponent))
                action->setToolTip(actionConfig.tooltip);

            if(components.testFlag(IconComponent))
                action->setIcon(IconController::icon(actionConfig.icon));

            if(components.testFlag(ShortcutComponent)) {
                QKeySequence shortcut = ActionManager::keyboardShortcut(actionID);
                action->setShortcut(shortcut);
            }

        } else if constexpr(std::is_base_of<QMenu, T>::value) {
            QMenu *menu = static_cast<QMenu *>(widget);
            _menus.insert(menu, QPair<QString, ActionComponents>(actionID, components));
            connect(menu, &QObject::destroyed, instance(), [menu](){ _menus.remove(menu); });

            if(components.testFlag(TextComponent))
                menu->setTitle(actionConfig.text);

            if(components.testFlag(TooltipComponent))
                menu->setToolTip(actionConfig.tooltip);

            if(components.testFlag(IconComponent))
                menu->setIcon(IconController::icon(actionConfig.icon));
        } else {
            qWarning() << "Cannot add action to ActionController. Unsupported type:" << typeid(T).name();
        }

        return widget;
    }

    template <typename T>
    static T *remove(T *widget);

    static QStringList globalActionIDs();

    template <typename T>
    static void setGlobalAction(const QString &actionID, T *widget) {
        qDebug() << "insert action" << actionID;

        if(_globalActions.contains(actionID))
            instance()->disconnect(qobject_cast<QObject *>(_globalActions[actionID].widget));

        _globalActions.insert(actionID, widget);
        connect(widget, &QObject::destroyed, instance(), [actionID](){ _globalActions.remove(actionID); });
    }

    static void executeGlobalAction(const QString &actionID) {
        if(!_globalActions.contains(actionID)) {
            qWarning() << "Global action" << actionID << "not found.";
            return;
        }

        _globalActions[actionID].execute();
    }

protected slots:
    void onIconSetChanged();
    void onActionShortcutChanged(const QString &keyboardShortcutID, const QKeySequence shortcut);

signals:

private:
    static inline QHash<QMenu *, QPair<QString, ActionComponents>> _menus;
    static inline QHash<QAction *, QPair<QString, ActionComponents>> _actions;
    static inline QHash<QAbstractButton *, QPair<QString, ActionComponents>> _buttons;

    static inline QHash<QString, GlobalActionWrapper> _globalActions;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ActionController::ActionComponents)

#endif // ACTIONCONTROLLER_H
