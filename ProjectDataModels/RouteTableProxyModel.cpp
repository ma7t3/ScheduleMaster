#include "RouteTableProxyModel.h"

#include "RouteTableModel.h"

#include "ProjectData/Route.h"

RouteTableProxyModel::RouteTableProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {}

bool RouteTableProxyModel::lessThan(const QModelIndex &sourceLeft,
                                    const QModelIndex &sourceRight) const {

    static const QList<int> customColumns = {0, 1, 5, 6, 7};
    if(!customColumns.contains(sortColumn()))
        return QSortFilterProxyModel::lessThan(sourceLeft, sourceRight);

    Route *left = static_cast<Route *>(sourceLeft.internalPointer());
    Route *right = static_cast<Route *>(sourceRight.internalPointer());

    Line *l = static_cast<RouteTableModel *>(sourceModel())->line();

    switch(sortColumn()) {
    case 0: return left->code() < right->code();
    case 1:
        return left->direction() == right->direction()
                   ? left->code() < right->code()
                   : l->indexOfDirection(left->direction())
                         < l->indexOfDirection(right->direction());
    case 5: return left->busstopCount() < right->busstopCount();
    default: return false; // TODO 6 + 7
    }

    return false;
}
