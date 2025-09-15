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

bool RouteTableProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    QModelIndex codeIndex         = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex nameIndex         = sourceModel()->index(sourceRow, 2, sourceParent);
    QModelIndex firstBusstopIndex = sourceModel()->index(sourceRow, 3, sourceParent);
    QModelIndex lastBusstopIndex  = sourceModel()->index(sourceRow, 4, sourceParent);

    QString code         = codeIndex.data().toString();
    QString name         = nameIndex.data().toString();
    QString firstBusstop = firstBusstopIndex.data().toString();
    QString lastBusstop  = lastBusstopIndex.data().toString();

    const QRegularExpression expr = filterRegularExpression();

    return expr.match(name).hasMatch() || expr.match(code).hasMatch()
           || expr.match(firstBusstop).hasMatch() || expr.match(lastBusstop).hasMatch();
}
