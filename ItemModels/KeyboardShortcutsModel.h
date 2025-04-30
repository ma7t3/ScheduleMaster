#ifndef KEYBOARDSHORTCUTSMODEL_H
#define KEYBOARDSHORTCUTSMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>

#include "Global/GlobalConfig.h"
#include "Global/LocalConfig.h"

class KeyboardShortcutsSortFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit KeyboardShortcutsSortFilterProxyModel(QObject *parent = nullptr);

public slots:
    void setFilterModifiedOnly(bool active);
    void setFilterText(const QString &text);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString _filterText;
    bool _filterModifiedOnly;
};

class KeyboardShortcutsModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit KeyboardShortcutsModel(QObject *parent = nullptr);

    enum CustomRoles {
        IDRole = 0x0100,
        ModifiedRole = 0x0101
    };

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    KeyboardShortcut metaData(const QModelIndex &index) const;
    QKeySequence shortcut(const QModelIndex &index) const;

    void setModifiedShortcut(const QString &id, const QKeySequence &keySequence);
    void setAllShortcutsToDefault();
    void saveShortcuts();

    void reload();

protected:
    int indexOf(const QString &id) const;

private:
    QList<QPair<KeyboardShortcut, QKeySequence>> _shortcuts;
    QHash<QString, int> _shortcutIndexes;

    QHash<QString, QKeySequence> _changedShortcuts;
};

#endif // KEYBOARDSHORTCUTSMODEL_H
