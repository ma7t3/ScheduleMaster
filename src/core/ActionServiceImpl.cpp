#include "ActionServiceImpl.h"

#include "SettingsServiceImpl.h"
#include "IconServiceImpl.h"
#include "AppearanceServiceImpl.h"

#include <QAction>
#include <QPushButton>
#include <QToolButton>
#include <QCommandLinkButton>
#include <QMenu>

namespace ScheduleMaster::Core {

ActionServiceImpl::ActionServiceImpl(QObject *parent) : GlobalConfigServiceCRTP(parent, "Actions") {
    initRepository();

    const QList<ActionConfig> itemList = repository()->items();
    for(const ActionConfig &shortcut : itemList) {
        if(!shortcut.canHaveShortcut)
            continue;

        SettingsItem item("keyboardShortcuts/" + shortcut.id());
        item.type         = QMetaType::QKeySequence;
        item.description  = shortcut.description;
        item.defaultValue = shortcut.defaultKeyboardShortcut;
        SettingsServiceImpl::instance()->registerSetting(item);
    }

    connect(static_cast<SettingsServiceImpl *>(SettingsServiceImpl::instance()),
            &SettingsServiceImpl::valueChanged,
            instance(),
            [this](const QString &settingID, const QVariant &value) {
                if(!settingID.startsWith("keyboardShortcuts/"))
                    return;

                QString shortcutID = settingID;
                const QKeySequence sequence = QKeySequence(value.toString());
                shortcutID.remove("keyboardShortcuts/");
                _keyboardShortcutsCache.insert(shortcutID, sequence);
                emit keyboardShortcutChanged(shortcutID, sequence);
            });

    connect(IconServiceImpl::instance(),
            &IconServiceImpl::currentIconSetChanged,
            this,
            &ActionServiceImpl::onIconSetChanged);

    connect(AppearanceServiceImpl::instance(),
            &AppearanceServiceImpl::currentStyleChanged,
            this,
            &ActionServiceImpl::onIconSetChanged);

    connect(this,
            &ActionServiceImpl::keyboardShortcutChanged,
            this,
            &ActionServiceImpl::onActionShortcutChanged);
}

QKeySequence ActionServiceImpl::keyboardShortcut(const QString &actionID) const {
    if(!repository()->itemExists(actionID) || !repository()->item(actionID).canHaveShortcut)
        return QKeySequence();

    if(_keyboardShortcutsCache.contains(actionID))
        return _keyboardShortcutsCache.value(actionID);

    const QString fullID = "keyboardShortcuts/" + actionID;

    if(SettingsServiceImpl::instance()->keyExists(fullID)) {
        const QKeySequence sequence = QKeySequence(SettingsServiceImpl::instance()->value(fullID).toString());
        _keyboardShortcutsCache.insert(actionID, sequence);
        return sequence;
    }

    const QKeySequence sequence = repository()->item(actionID).defaultKeyboardShortcut;
    const_cast<ActionServiceImpl *>(this)->setKeyboardShortcut(actionID, sequence);
    return sequence;
}

bool ActionServiceImpl::shortcutIsDefault(const QString &actionID,
                                          const QKeySequence &sequence) const {
    return repository()->itemExists(actionID)
               ? repository()->item(actionID).defaultKeyboardShortcut == sequence
               : false;
}

void ActionServiceImpl::setKeyboardShortcut(const QString &actionID, const QKeySequence shortcut) {
    SettingsServiceImpl::instance()->setValue("keyboardShortcuts/" + actionID,
                                              shortcut.toString(QKeySequence::PortableText));

    _keyboardShortcutsCache.insert(actionID, shortcut);
}

void ActionServiceImpl::importKeyboardShortcuts(const QJsonArray &jsonArray) {
    for(const QJsonValue &value : jsonArray) {
        const QJsonObject jObj        = value.toObject();
        const QString     id          = jObj.value("id").toString();
        const QString     keySequence = jObj.value("keySequence").toString();
        setKeyboardShortcut(id, keySequence);
    }
}

QJsonArray ActionServiceImpl::exportKeyboardShortcuts() const {
    QJsonArray array;
    const QStringList keys = SettingsServiceImpl::instance()->keysInGroup("keyboardShortcuts");
    for(const QString &id : keys) {
        QJsonObject obj;
        obj["id"] = id;
        obj["keySequence"] = keyboardShortcut(id).toString();
        array << obj;
    }
    return array;
}

QList<ActionConfig> ActionServiceImpl::actions() const {
    return repository()->items();
}

QMap<QString, ActionConfig> ActionServiceImpl::actionsMap() const {
    return repository()->itemsMap();
}

QStringList ActionServiceImpl::actionIDs() const {
    return repository()->itemIDs();
}

ActionConfig ActionServiceImpl::action(const QString &actionID) const {
    return repository()->item(actionID);
}

bool ActionServiceImpl::actionExists(const QString &actionID) const {
    return repository()->itemExists(actionID);
}

bool ActionServiceImpl::registerAction(const ActionConfig &actionConfig) {
    if(!repository()->addItem(actionConfig))
        return false;

    if(actionConfig.canHaveShortcut) {
        SettingsItem settingsItem("keyboardShortcuts/" + actionConfig.id());
        settingsItem.type         = QMetaType::QKeySequence;
        settingsItem.description  = actionConfig.description;
        settingsItem.defaultValue = actionConfig.defaultKeyboardShortcut;
        SettingsServiceImpl::instance()->registerSetting(settingsItem);
    }
    return true;
}

QAction *ActionServiceImpl::createAction(const QString &actionID,
                                         const ActionComponents &components, QObject *parent) {
    return addAction(new QAction(parent), actionID, components);
}

QMenu *ActionServiceImpl::createMenu(const QString &actionID, const ActionComponents &components,
                                     QWidget *parent) {
    return addMenu(new QMenu(parent), actionID, components);
}

QPushButton *ActionServiceImpl::createPushButton(const QString &actionID,
                                                 const ActionComponents &components,
                                                 QWidget *parent) {
    return static_cast<QPushButton *>(addButton(new QPushButton(parent), actionID, components));
}

QToolButton *ActionServiceImpl::createToolButton(const QString &actionID,
                                                 const ActionComponents &components,
                                                 QWidget *parent) {
    return static_cast<QToolButton *>(addButton(new QToolButton(parent), actionID, components));
}

QCommandLinkButton *ActionServiceImpl::createCommandLinkButton(const QString &actionID,
                                                               const ActionComponents &components,
                                                               QWidget *parent) {
    return static_cast<QCommandLinkButton *>(addButton(new QCommandLinkButton(parent), actionID, components));
}

QAbstractButton *ActionServiceImpl::addButton(QAbstractButton *button, const QString &actionID,
                                  const ActionComponents &components) {
    if(!actionExists(actionID)) {
        qWarning() << "cannot add action for actionID" << actionID
                   << "- action does not exist.";
        return button;
    }

    ActionConfig actionConfig = repository()->item(actionID);
    _buttons.insert(button, QPair<QString, ActionComponents>(actionID, components));
    connect(button, &QObject::destroyed, instance(), [this, button]() {
        _buttons.remove(button);
    });

    if(components.testFlag(TextComponent))
        button->setText(actionConfig.text);

    if(components.testFlag(TooltipComponent))
        button->setToolTip(actionConfig.tooltip.isEmpty()? actionConfig.description : actionConfig.tooltip);

    if(components.testFlag(IconComponent))
        button->setIcon(IconServiceImpl::instance()->icon(actionConfig.icon));

    if(components.testFlag(ShortcutComponent)) {
        const QKeySequence shortcut = keyboardShortcut(actionID);
        button->setShortcut(shortcut);
    }

    return button;
}

QAction *ActionServiceImpl::addAction(QAction *action, const QString &actionID,
                                  const ActionComponents &components) {
    if(!actionExists(actionID)) {
        qWarning() << "cannot add action for actionID" << actionID
                   << "- action does not exist.";
        return action;
    }

    ActionConfig actionConfig = repository()->item(actionID);
    _actions.insert(action, QPair<QString, ActionComponents>(actionID, components));
    connect(action, &QObject::destroyed, instance(), [this, action]() {
        _actions.remove(action);
    });

    if(components.testFlag(TextComponent))
        action->setText(actionConfig.text);

    if(components.testFlag(TooltipComponent))
        action->setToolTip(actionConfig.tooltip.isEmpty()? actionConfig.description : actionConfig.tooltip);

    if(components.testFlag(IconComponent))
        action->setIcon(IconServiceImpl::instance()->icon(actionConfig.icon));

    if(components.testFlag(ShortcutComponent)) {
        QKeySequence shortcut = keyboardShortcut(actionID);
        action->setShortcut(shortcut);
    }

    return action;
}

QMenu *ActionServiceImpl::addMenu(QMenu *menu, const QString &actionID,
                                const ActionComponents &components) {
    if(!actionExists(actionID)) {
        qWarning() << "cannot add action for actionID" << actionID
                   << "- action does not exist.";
        return menu;
    }

    ActionConfig actionConfig = repository()->item(actionID);
    _menus.insert(menu, QPair<QString, ActionComponents>(actionID, components));
    connect(menu, &QObject::destroyed, instance(), [this, menu]() { _menus.remove(menu); });

    if(components.testFlag(TextComponent))
        menu->setTitle(actionConfig.text);

    if(components.testFlag(TooltipComponent))
        menu->setToolTip(actionConfig.tooltip);

    if(components.testFlag(IconComponent))
        menu->setIcon(IconServiceImpl::instance()->icon(actionConfig.icon));

    return menu;

}

void ActionServiceImpl::addSyncedActionAndButton(QAction *action, QPushButton *button,
                                                 const QString &actionID,
                                                 const ActionComponents &actionComponents,
                                                 const ActionComponents &buttonComponents) {
    addAction(action, actionID, actionComponents);
    addButton(button, actionID, buttonComponents);

    connect(action, &QAction::enabledChanged, button, &QPushButton::setEnabled);
    connect(button, &QPushButton::clicked,    action, &QAction::trigger);
}

void ActionServiceImpl::removeButton(QAbstractButton *button) {
    _buttons.remove(button);
}

void ActionServiceImpl::removeAction(QAction *action) {
    _actions.remove(action);
}

void ActionServiceImpl::removeMenu(QMenu *menu) {
    _menus.remove(menu);
}

QStringList ActionServiceImpl::globalActionIDs() const {
    return _globalActions.keys();
}

GlobalActionWrapper ActionServiceImpl::globalAction(const QString &actionID) const {
    return _globalActions.value(actionID);
}

void ActionServiceImpl::setGlobalAction(const QString &actionID, QObject *widget) {
    if(_globalActions.contains(actionID))
        instance()->disconnect(qobject_cast<QObject *>(_globalActions[actionID].widget));

    _globalActions.insert(actionID, GlobalActionWrapper(widget));
    connect(widget, &QObject::destroyed, instance(), [this, actionID]() {
        _globalActions.remove(actionID);
    });
}

void ActionServiceImpl::onIconSetChanged() {
    for(auto it = _actions.begin(); it != _actions.end(); ++it) {
        if(!it.value().second.testFlag(IconComponent))
            continue;

        QString actionID = it.value().first;
        ActionConfig actionConfig = repository()->item(actionID);
        it.key()->setIcon(IconServiceImpl::instance()->icon(actionConfig.icon));
    }

    for(auto it = _menus.begin(); it != _menus.end(); ++it) {
        if(!it.value().second.testFlag(IconComponent))
            continue;

        QString actionID = it.value().first;
        ActionConfig actionConfig = repository()->item(actionID);
        it.key()->setIcon(IconServiceImpl::instance()->icon(actionConfig.icon));
    }

    for(auto it = _buttons.begin(); it != _buttons.end(); ++it) {
        if(!it.value().second.testFlag(IconComponent))
            continue;

        QString actionID = it.value().first;
        ActionConfig actionConfig = repository()->item(actionID);
        it.key()->setIcon(IconServiceImpl::instance()->icon(actionConfig.icon));
    }
}

void ActionServiceImpl::onActionShortcutChanged(const QString &keyboardShortcutID,
                                                const QKeySequence shortcut) {
    for(auto it = _actions.begin(); it != _actions.end(); ++it) {
        if(it.value().first != keyboardShortcutID
           || !it.value().second.testFlag(ShortcutComponent))
            continue;

        if(it.key()->shortcut() != shortcut)
            it.key()->setShortcut(shortcut);
    }

    for(auto it = _buttons.begin(); it != _buttons.end(); ++it) {
        if(it.value().first != keyboardShortcutID
           || !it.value().second.testFlag(ShortcutComponent))
            continue;

        if(it.key()->shortcut() != shortcut)
            it.key()->setShortcut(shortcut);
    }

    if(_globalActions.contains(keyboardShortcutID))
        _globalActions[keyboardShortcutID].shortcut = shortcut;
}
}