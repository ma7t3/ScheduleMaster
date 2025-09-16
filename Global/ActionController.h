#ifndef ACTIONCONTROLLER_H
#define ACTIONCONTROLLER_H

#include <QCommandLinkButton>
#include <QMenu>
#include <QObject>
#include <QToolButton>

#include "Global/ActionManager.h"
#include "Global/IconController.h"
#include "Global/Singleton.h"

/**
 * @brief The GlobalActionWrapper struct is a wrapper around an action that can be used as a global action.
 *
 * It contains a pointer to the action, and the action's shortcut.
 */
struct GlobalActionWrapper {
    GlobalActionWrapper(QObject *widget = nullptr);

    /**
     * @brief Executes the action.
     *
     * Depending on the type of the wrapped widget, it calls the click() method
     * or the trigger() method.
     */
    void execute();

    /// The widget (usually a QAction or button) that is associated with the global action
    QObject *widget;

    /// The shortcut that is used to trigger the global action
    QKeySequence shortcut;
};

/**
 * @brief The ActionController class is responsible for connecting and controlling all actions in the application.
 *
 * It connects QActions, buttons etc. to the actions defined in the ActionManager allowing to keep icons, shortcuts etc. up to date.
 *
 * It also provides a way to set global actions, which are actions that are present in the application's global command search.
 *
 * The ActionController is a singleton class, meaning that it can be accessed from anywhere in the application.
 */
class ActionController : public Singleton<ActionController> {
    friend class Singleton<ActionController>;
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ActionController);

private:
    ActionController() : Singleton<ActionController>() {}

public:
    /**
     * @brief Enum for the components of an action.
     */
    enum ActionComponent {
        NoComponents = 0x0, ///< No component
        IconComponent = 0x1, ///< The icon of the action
        TextComponent = 0x4, ///< The text of the action
        TooltipComponent = 0x8, ///< The tooltip of the action
        ShortcutComponent = 0x10, ///< The shortcut of the action
        AllComponents = IconComponent | TextComponent | TooltipComponent | ShortcutComponent, ///< All components
        AllExceptIconComponent = AllComponents & ~IconComponent, ///< All components except the icon
        AllExceptTextComponent = AllComponents & ~TextComponent, ///< All components except the text
        AllExceptTooltipComponent = AllComponents & ~TooltipComponent, ///< All components except the tooltip
        AllExceptShortcutComponent = AllComponents & ~ShortcutComponent, ///< All components except the shortcut
    };
    Q_DECLARE_FLAGS(ActionComponents, ActionComponent)

    /**
     * @brief Initializes the ActionController.
     */
    static void init();

    /**
     * @brief Creates a new QAction with the given actionID and components.
     *
     * @param actionID The ID of the action.
     * @param components The components of the action to create.
     * @param parent The parent of the action.
     * @return The created action.
     */
    static QAction *createAction(const QString &actionID,
                                 const ActionComponents &components = AllComponents,
                                 QObject *parent = nullptr);

    /**
     * @brief Creates a QMenu with the given actionID and components.
     *
     * @param actionID The ID of the menu.
     * @param components The components of the menu to create.
     * @param parent The parent of the menu.
     * @return The created menu.
     */
    static QMenu *createMenu(const QString &actionID,
                             const ActionComponents &components = AllComponents,
                             QObject *parent = nullptr);

    /**
     * @brief Creates a QPushButton with the given actionID and components.
     *
     * @param actionID The ID of the button.
     * @param components The components of the button to create.
     * @param parent The parent of the button.
     * @return The created button.
     */
    static QPushButton *createPushButton(const QString &actionID,
                                         const ActionComponents &components = AllComponents,
                                         QWidget *parent = nullptr);

    /**
     * @brief Creates a QToolButton with the given actionID and components.
     *
     * @param actionID The ID of the button.
     * @param components The components of the button to create.
     * @param parent The parent of the button.
     * @return The created button.
     */
    static QToolButton *createToolButton(const QString &actionID,
                                         const ActionComponents &components = AllComponents,
                                         QWidget *parent = nullptr);

    /**
     * @brief Creates a QCommandLinkButton with the given actionID and components.
     *
     * @param actionID The ID of the button.
     * @param components The components of the button to create.
     * @param parent The parent of the button.
     * @return The created button.
     */
    static QCommandLinkButton *createCommandLinkButton(
        const QString &actionID, const ActionComponents &components = AllComponents,
        QWidget *parent = nullptr);

    /**
     * @brief Adds the given widget as an action to the ActionController.
     *
     * @param widget The widget to add.
     * @param actionID The ID of the action.
     * @param components The components of the action to add.
     * @return The added widget.
     */
    template<typename T>
    static T *add(T *widget, const QString &actionID,
                  const ActionComponents &components = AllComponents) {
        if(!ActionManager::itemExists(actionID)) {
            qWarning() << "cannot add action for actionID" << actionID
                       << "- action does not exist in ActionManager.";
            return widget;
        }

        ActionConfig actionConfig = ActionManager::item(actionID);

        if constexpr(std::is_base_of<QAbstractButton, T>::value) {
            QAbstractButton *button = static_cast<QAbstractButton *>(widget);
            _buttons.insert(button, QPair<QString, ActionComponents>(actionID, components));
            connect(button, &QObject::destroyed, instance(), [button]() {
                _buttons.remove(button);
            });

            if(components.testFlag(TextComponent))
                button->setText(actionConfig.text);

            if(components.testFlag(TooltipComponent))
                button->setToolTip(actionConfig.tooltip.isEmpty()? actionConfig.description : actionConfig.tooltip);

            if(components.testFlag(IconComponent))
                button->setIcon(IconController::icon(actionConfig.icon));

            if(components.testFlag(ShortcutComponent)) {
                QKeySequence shortcut = ActionManager::keyboardShortcut(actionID);
                button->setShortcut(shortcut);
            }

        } else if constexpr(std::is_base_of<QAction, T>::value) {
            QAction *action = static_cast<QAction *>(widget);
            _actions.insert(action, QPair<QString, ActionComponents>(actionID, components));
            connect(action, &QObject::destroyed, instance(), [action]() {
                _actions.remove(action);
            });

            if(components.testFlag(TextComponent))
                action->setText(actionConfig.text);

            if(components.testFlag(TooltipComponent))
                action->setToolTip(actionConfig.tooltip.isEmpty()? actionConfig.description : actionConfig.tooltip);

            if(components.testFlag(IconComponent))
                action->setIcon(IconController::icon(actionConfig.icon));

            if(components.testFlag(ShortcutComponent)) {
                QKeySequence shortcut = ActionManager::keyboardShortcut(actionID);
                action->setShortcut(shortcut);
            }

        } else if constexpr(std::is_base_of<QMenu, T>::value) {
            QMenu *menu = static_cast<QMenu *>(widget);
            _menus.insert(menu, QPair<QString, ActionComponents>(actionID, components));
            connect(menu, &QObject::destroyed, instance(), [menu]() { _menus.remove(menu); });

            if(components.testFlag(TextComponent))
                menu->setTitle(actionConfig.text);

            if(components.testFlag(TooltipComponent))
                menu->setToolTip(actionConfig.tooltip);

            if(components.testFlag(IconComponent))
                menu->setIcon(IconController::icon(actionConfig.icon));
        } else {
            qWarning() << "Cannot add action to ActionController. Unsupported type:"
                       << typeid(T).name();
        }

        return widget;
    }

    static void addSyncedActionAndButton(QAction *action, QPushButton *button,
                                         const QString &actionID,
                                         const ActionComponents &actionComponents = AllComponents,
                                         const ActionComponents &buttonComponents = AllComponents) {
        add(action, actionID, actionComponents);
        add(button, actionID, buttonComponents);

        connect(action, &QAction::enabledChanged, button, &QPushButton::setEnabled);
        connect(button, &QPushButton::clicked,    action, &QAction::trigger);
    }

    /**
     * @brief Adds the given widget (usually a QAction or button) as a global action to the ActionController.
     *
     * This is a convenience function that adds the widget as an action to the
     * ActionController and sets it as a global action. The widget is added with
     * the given actionID and components.
     *
     * This means that when the action specified by actionID is called from the global command search,
     * the widget will be clicked or triggered.
     *
     * This is unique which means, if another widget is added with the same actionID, the previous widget will be removed.
     *
     * @param widget The widget to add.
     * @param actionID The ID of the action.
     * @param components The components of the action to add.
     * @return The added widget.
     */
    template<typename T>
    static T *addAsGlobalAction(T *widget, const QString &actionID,
                                const ActionComponents &components = AllComponents) {
        add(widget, actionID, components);
        setGlobalAction(actionID, widget);
        return widget;
    }

    /**
     * @brief Removes the given widget from the ActionController.
     *
     * @param widget The widget to remove.
     * @return The removed widget.
     */
    template<typename T>
    static T *remove(T *widget) {
        if constexpr(std::is_base_of<QAbstractButton, T>::value)
            _buttons.remove(widget);

        else if constexpr(std::is_base_of<QAction, T>::value)
            _actions.remove(widget);
    }

    /**
     * @brief Returns the IDs of all global actions.
     *
     * @return The IDs of all global actions.
     */
    static QStringList globalActionIDs();

    /**
     * @brief Returns the global action with the given actionID.
     *
     * @param actionID The ID of the global action.
     * @return The global action with the given actionID.
     */
    static GlobalActionWrapper globalAction(const QString &actionID);

    /**
     * @brief Sets the global action with the given actionID to the given widget.
     * 
     * This is unique which means, if another widget is added with the same actionID, the previous widget will be removed.
     *
     * @param actionID The ID of the global action.
     * @param widget The widget to set as the global action.
     */
    static void setGlobalAction(const QString &actionID, QObject *widget);

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
