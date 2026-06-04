#ifndef ACTIONSERVICEIMPL_H
#define ACTIONSERVICEIMPL_H

#include "IActionService.h"
#include "GlobalConfigService.h"
#include "ActionRepository.h"

namespace ScheduleMaster::Core {

class ActionServiceImpl : public GlobalConfigServiceCRTP<ActionRepository, ActionServiceImpl>, public IActionService {
    Q_OBJECT
public:
    ActionServiceImpl(QObject *parent);

    virtual QKeySequence keyboardShortcut(const QString &actionID) const override;
    virtual bool shortcutIsDefault(const QString &actionID,
                                   const QKeySequence &sequence) const override;
    virtual void setKeyboardShortcut(const QString &actionID, const QKeySequence shortcut) override;

    virtual void importKeyboardShortcuts(const QJsonArray &jsonArray) override;
    virtual QJsonArray exportKeyboardShortcuts() const override;

    virtual QList<ActionConfig> actions() const override;
    virtual QMap<QString, ActionConfig> actionsMap() const override;
    virtual QStringList actionIDs() const override;
    virtual ActionConfig action(const QString &actionID) const override;
    virtual bool actionExists(const QString &actionID) const override;
    virtual bool registerAction(const ActionConfig &actionConfig) override;

    virtual QAction *createAction(const QString &actionID,
                                  const ActionComponents &components = AllComponents,
                                  QObject *parent = nullptr) override;

    virtual QMenu *createMenu(const QString &actionID,
                              const ActionComponents &components = AllComponents,
                              QWidget *parent = nullptr) override;

    virtual QPushButton *createPushButton(const QString &actionID,
                                          const ActionComponents &components = AllComponents,
                                          QWidget *parent = nullptr) override;

    virtual QToolButton *createToolButton(const QString &actionID,
                                          const ActionComponents &components = AllComponents,
                                          QWidget *parent = nullptr) override;

    virtual QCommandLinkButton *createCommandLinkButton(
        const QString &actionID, const ActionComponents &components = AllComponents,
        QWidget *parent = nullptr) override;

    virtual QAbstractButton *addButton(QAbstractButton *button, const QString &actionID,
                           const ActionComponents &components = AllComponents) override;

    virtual QAction *addAction(QAction *action, const QString &actionID,
                           const ActionComponents &components = AllComponents) override;

    virtual QMenu *addMenu(QMenu *menu, const QString &actionID,
                         const ActionComponents &components = AllComponents) override;

    virtual void addSyncedActionAndButton(
        QAction *action, QPushButton *button, const QString &actionID,
        const ActionComponents &actionComponents = AllComponents,
        const ActionComponents &buttonComponents = AllComponents) override;

    virtual void removeButton(QAbstractButton *button) override;
    virtual void removeAction(QAction *action) override;
    virtual void removeMenu(QMenu *menu) override;

    virtual QStringList globalActionIDs() const override;
    virtual GlobalActionWrapper globalAction(const QString &actionID) const override;
    virtual void setGlobalAction(const QString &actionID, QObject *widget) override;

protected slots:
    void onIconSetChanged();
    void onActionShortcutChanged(const QString &keyboardShortcutID, const QKeySequence shortcut);

signals:
    void keyboardShortcutChanged(const QString &keyboardShortcutID, const QKeySequence &shortcut);

private:
    QHash<QMenu *, QPair<QString, ActionComponents>> _menus;
    QHash<QAction *, QPair<QString, ActionComponents>> _actions;
    QHash<QAbstractButton *, QPair<QString, ActionComponents>> _buttons;
    QHash<QString, GlobalActionWrapper> _globalActions;

    mutable QHash<QString, QKeySequence> _keyboardShortcutsCache;
};

}

#endif // ACTIONSERVICEIMPL_H
