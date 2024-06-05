#include "SimpleCustomBusstopListModel.h"

#include <QFont>

SimpleCustomBusstopListModel::SimpleCustomBusstopListModel(QObject *parent) : ProjectDataRowBasedTableModel(parent) {}

QVariant SimpleCustomBusstopListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    Busstop *b = _items[index.row()];

    switch(role) {
    case Qt::DisplayRole:
        return b->name();

    case Qt::FontRole:
        QFont f;
        if(b->isImportant())
            f.setBold(true);
        return f;
    }

    return QVariant();
}

void SimpleCustomBusstopListModel::addBusstop(Busstop *b, const int &index) {
    int newIndex = index;
    if(index == -1)
        newIndex = _items.count();
    QAbstractTableModel::beginInsertRows(QModelIndex(), newIndex, newIndex);
    _items.insert(newIndex, b);
    QAbstractTableModel::endInsertRows();
}

void SimpleCustomBusstopListModel::removeBusstop(Busstop *b) {
    int index = _items.indexOf(b);
    if(index == -1)
        return;

    removeBusstop(index);
}

void SimpleCustomBusstopListModel::removeBusstop(const int &index) {
    QAbstractItemModel::beginRemoveRows(QModelIndex(), index, index);
    _items.remove(index);
    QAbstractItemModel::endRemoveRows();
}

void SimpleCustomBusstopListModel::setBusstops(QList<Busstop *> list) {
    QAbstractTableModel::beginResetModel();
    _items = list;
    QAbstractTableModel::endResetModel();
    emit ProjectDataTableSignals::updateFinished();
}
