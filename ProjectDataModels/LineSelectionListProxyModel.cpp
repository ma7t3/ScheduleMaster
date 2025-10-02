#include "LineSelectionListProxyModel.h"

#include "ProjectData/Line.h"

LineSelectionListProxyModel::LineSelectionListProxyModel(QObject *parent) :
    QSortFilterProxyModel{parent} {}

bool LineSelectionListProxyModel::lessThan(const QModelIndex &left,
                                           const QModelIndex &right) const {
    switch(sortColumn()) {
    case 1: {
        Line *a = static_cast<Line *>(left.internalPointer());
        Line *b = static_cast<Line *>(right.internalPointer());
        return *a < *b;
    }
    default:
        return QSortFilterProxyModel::lessThan(left, right);
    }
}
