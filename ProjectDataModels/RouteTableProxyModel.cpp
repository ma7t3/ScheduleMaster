#include "RouteTableProxyModel.h"

#include "RouteTableModel.h"

#include "src/projectdata/Route.h"

#include <QWidget>

RouteTableProxyModel::RouteTableProxyModel(QAbstractButton *popupButton, QObject *parent) : SortFilterProxyModel<WdgRouteFilterPopup>(popupButton, parent) {}

bool RouteTableProxyModel::lessThan(const QModelIndex &sourceLeft,
                                    const QModelIndex &sourceRight) const {
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
    case 5: return left->busstopCount() < right->busstopCount(); // TODO 6 + 7
    default: return QSortFilterProxyModel::lessThan(sourceLeft, sourceRight);
    }

    return false;
}

bool RouteTableProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    Route *r = static_cast<Route *>(sourceModel()->index(sourceRow, 0).internalPointer());
    LineDirection *filterDirection = filterPopup()->filterDirection();
    Busstop *filterFirstBusstop = filterPopup()->filterFirstBusstop();
    Busstop *filterLastBusstop = filterPopup()->filterLastBusstop();

    if(filterDirection && r->direction() != filterDirection)
        return false;

    if(filterFirstBusstop && r->firstBusstop()->busstop() != filterFirstBusstop)
        return false;

    if(filterLastBusstop && r->lastBusstop()->busstop() != filterLastBusstop)
        return false;

    const QModelIndex codeIndex         = sourceModel()->index(sourceRow, 0, sourceParent);
    const QModelIndex nameIndex         = sourceModel()->index(sourceRow, 2, sourceParent);
    const QModelIndex firstBusstopIndex = sourceModel()->index(sourceRow, 3, sourceParent);
    const QModelIndex lastBusstopIndex  = sourceModel()->index(sourceRow, 4, sourceParent);

    const QString code         = codeIndex.data().toString();
    const QString name         = nameIndex.data().toString();
    const QString firstBusstop = firstBusstopIndex.data().toString();
    const QString lastBusstop  = lastBusstopIndex.data().toString();

    const QRegularExpression expr = filterRegularExpression();

    return expr.match(name).hasMatch() || expr.match(code).hasMatch()
           || expr.match(firstBusstop).hasMatch() || expr.match(lastBusstop).hasMatch();
}
