#include "KeyboardShortcutsModel.h"

#include <QIcon>
#include <QFont>

KeyboardShortcutsSortFilterProxyModel::KeyboardShortcutsSortFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    _filterModifiedOnly(false) {}


void KeyboardShortcutsSortFilterProxyModel::setFilterModifiedOnly(bool active) {
    _filterModifiedOnly = active;
    invalidateFilter();
}

void KeyboardShortcutsSortFilterProxyModel::setFilterText(const QString &text) {
    _filterText = text;
    invalidateFilter();
}

bool KeyboardShortcutsSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    bool changed = sourceModel()->index(sourceRow, 0, sourceParent).data(0x0101).toBool();
    if(_filterModifiedOnly && !changed)
        return false;

    QString name     = sourceModel()->index(sourceRow, 0, sourceParent).data().toString();
    QString id       = sourceModel()->index(sourceRow, 0, sourceParent).data(0x100).toString();
    QString shortcut = sourceModel()->index(sourceRow, 1, sourceParent).data().toString();

    return name.contains(_filterText, Qt::CaseInsensitive) || id.contains(_filterText, Qt::CaseInsensitive) || shortcut.contains(_filterText, Qt::CaseInsensitive);
}


KeyboardShortcutsModel::KeyboardShortcutsModel(QObject *parent) :
    QAbstractTableModel(parent) {
    reload();
}

QVariant KeyboardShortcutsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(orientation == Qt::Vertical || role != Qt::DisplayRole)
        return QVariant();

    switch(section) {
        case 0: return tr("Action");
        case 1: return tr("Keyboard Shortcut");
    }

    return QVariant();
}

int KeyboardShortcutsModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return _shortcuts.count();
}

int KeyboardShortcutsModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant KeyboardShortcutsModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || (
            role != Qt::DisplayRole &&
            role != Qt::DecorationRole &&
            role != Qt::ToolTipRole &&
            role != Qt::FontRole &&
            role != IDRole &&
            role != ModifiedRole
            )
        )
        return QVariant();

    const int row = index.row();
    const QPair<KeyboardShortcutConfig, QKeySequence> shortcut = _shortcuts[row];

    switch(role) {
        case Qt::DisplayRole: switch(index.column()) {
            case 0: return shortcut.first.description;
            case 1: return shortcut.second.toString(QKeySequence::NativeText);
        } break;
        case Qt::DecorationRole: switch(index.column()) {
            case 0: return QIcon(shortcut.first.icon);
        } break;
        case Qt::ToolTipRole: switch(index.column()) {
            case 0: return QString("<p><b>%1</b></p><p><small>%2</small></p>").arg(shortcut.first.description, shortcut.first.id());
            } break;
        case Qt::FontRole: switch (index.column()) {
            case 1: return shortcut.first.defaultKeySequence == shortcut.second ? QVariant() : QFont("", -1, QFont::Bold);
        } break;
        case IDRole: return shortcut.first.id();
        case ModifiedRole: return shortcut.first.defaultKeySequence != shortcut.second;
    }

    return QVariant();
}

KeyboardShortcutConfig KeyboardShortcutsModel::metaData(const QModelIndex &index) const {
    if(!index.isValid() || index.parent().isValid())
        return KeyboardShortcutConfig();

    return _shortcuts[index.row()].first;
}

QKeySequence KeyboardShortcutsModel::shortcut(const QModelIndex &index) const {
    if(!index.isValid() || index.parent().isValid())
        return QKeySequence();

    return _shortcuts[index.row()].second;
}

void KeyboardShortcutsModel::setModifiedShortcut(const QString &id, const QKeySequence &keySequence) {
    _changedShortcuts.insert(id, keySequence);
    int row = indexOf(id);
    if(row == -1)
        return;

    _shortcuts[row].second = keySequence;
    emit dataChanged(index(row, 0), index(row, 1), {Qt::DisplayRole});
}

void KeyboardShortcutsModel::setAllShortcutsToDefault() {
    for(const QPair<KeyboardShortcutConfig, QKeySequence> &shortcut : std::as_const(_shortcuts))
        if(shortcut.first.defaultKeySequence != shortcut.second)
            setModifiedShortcut(shortcut.first.id(), shortcut.first.defaultKeySequence);
}

void KeyboardShortcutsModel::saveShortcuts() {
    for (QHash<QString, QKeySequence>::const_iterator it = _changedShortcuts.constBegin(); it != _changedShortcuts.constEnd(); ++it)
        KeyboardShortcutManager::setKeyboardShortcut(it.key(), it.value());
}

void KeyboardShortcutsModel::reload() {
    _shortcuts.clear();
    _changedShortcuts.clear();

    beginResetModel();
    const QList<KeyboardShortcutConfig> metaDataList = KeyboardShortcutManager::items();
    int i = 0;
    for(const KeyboardShortcutConfig &shortcut : metaDataList) {
        QKeySequence keySequence = KeyboardShortcutManager::keyboardShortcut(shortcut.id());
        _shortcuts << QPair<KeyboardShortcutConfig, QKeySequence>(shortcut, keySequence);
        _shortcutIndexes.insert(shortcut.id(), i++);
    }
    endResetModel();
}

int KeyboardShortcutsModel::indexOf(const QString &id) const {
    return !_shortcutIndexes.contains(id) ? -1 : _shortcutIndexes.value(id);
}
