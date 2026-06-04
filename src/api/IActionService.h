#ifndef IACTIONSERVICE_H
#define IACTIONSERVICE_H

#include "ScheduleMaster_global.h"
#include "GlobalConfigItem.h"

#include <QKeySequence>

class QAction;
class QMenu;
class QAbstractButton;
class QPushButton;
class QToolButton;
class QCommandLinkButton;

namespace ScheduleMaster {

struct GlobalActionWrapper {
    GlobalActionWrapper(QObject *widget = nullptr);
    void execute();
    QObject *widget;
    QKeySequence shortcut;
};

class ActionConfig : public GlobalConfigItem {
public:
    ActionConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    ActionConfig(const QString &id, const int &index = 0);

    bool canHaveShortcut;
    QString text;
    QString tooltip;
    QString description;
    QString icon;
    QStringList breadcrumb;
    QKeySequence defaultKeyboardShortcut;
};


class SCHEDULEMASTERINTERFACE_EXPORT IActionService {
public:
    virtual ~IActionService() = default;

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

    virtual QKeySequence keyboardShortcut(const QString &actionID) const = 0;
    virtual bool shortcutIsDefault(const QString &actionID, const QKeySequence &sequence) const = 0;
    virtual void setKeyboardShortcut(const QString &actionID, const QKeySequence shortcut) = 0;

    virtual void importKeyboardShortcuts(const QJsonArray &jsonArray) = 0;
    virtual QJsonArray exportKeyboardShortcuts() const = 0;

    virtual QList<ActionConfig> actions() const = 0;
    virtual QMap<QString, ActionConfig> actionsMap() const = 0;
    virtual QStringList actionIDs() const = 0;
    virtual ActionConfig action(const QString &actionID) const = 0;
    virtual bool actionExists(const QString &actionID) const = 0;
    virtual bool registerAction(const ActionConfig &actionConfig) = 0;

    virtual QAction *createAction(const QString &actionID,
                                  const ActionComponents &components = AllComponents,
                                  QObject *parent = nullptr) = 0;

    virtual QMenu *createMenu(const QString &actionID,
                              const ActionComponents &components = AllComponents,
                              QWidget *parent = nullptr) = 0;

    virtual QPushButton *createPushButton(const QString &actionID,
                                          const ActionComponents &components = AllComponents,
                                          QWidget *parent = nullptr) = 0;

    virtual QToolButton *createToolButton(const QString &actionID,
                                          const ActionComponents &components = AllComponents,
                                          QWidget *parent = nullptr) = 0;

    virtual QCommandLinkButton *createCommandLinkButton(
        const QString &actionID, const ActionComponents &components = AllComponents,
        QWidget *parent = nullptr)
        = 0;

    virtual QAbstractButton *addButton(QAbstractButton *button, const QString &actionID,
                           const ActionComponents &components = AllComponents)
        = 0;

    virtual QAction *addAction(QAction *action, const QString &actionID,
                           const ActionComponents &components = AllComponents)
        = 0;

    virtual QMenu *addMenu(QMenu *menu, const QString &actionID,
                         const ActionComponents &components = AllComponents)
        = 0;

    virtual void addSyncedActionAndButton(QAction *action, QPushButton *button,
                                          const QString &actionID,
                                          const ActionComponents &actionComponents = AllComponents,
                                          const ActionComponents &buttonComponents = AllComponents)
        = 0;

    virtual void removeButton(QAbstractButton *button) = 0;
    virtual void removeAction(QAction *action) = 0;
    virtual void removeMenu(QMenu *menu) = 0;

    virtual QStringList globalActionIDs() const = 0;
    virtual GlobalActionWrapper globalAction(const QString &actionID) const = 0;
    virtual void setGlobalAction(const QString &actionID, QObject *widget) = 0;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IActionService::ActionComponents)

}

#endif // IACTIONSERVICE_H
