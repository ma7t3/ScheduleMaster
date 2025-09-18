#include "BusstopTableProxyModel.h"

#include "ProjectData/ProjectData.h"

BusstopTableProxyModel::BusstopTableProxyModel(QObject *parent) : SortFilterProxyModel(parent) {}

bool BusstopTableProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    Busstop *leftB  = static_cast<Busstop *>(left.internalPointer());
    Busstop *rightB = static_cast<Busstop *>(right.internalPointer());

    switch(sortColumn()) {
    case 0: return *leftB < *rightB;
    case 1: {
        return leftB->flags() == rightB->flags() ? *leftB < *rightB
                                                 : leftB->flags() > rightB->flags();
    }
    case 2: {
        return leftB->platformCount() == rightB->platformCount()
                   ? *leftB < *rightB
                   : leftB->platformCount() > rightB->platformCount();
        }
    case 3: {
        ProjectData *pd = leftB->findParent<ProjectData>();
        const int leftCount  = pd->linesAtBusstop(leftB).count();
        const int rightCount = pd->linesAtBusstop(rightB).count();
        return leftCount == rightCount ? *leftB < *rightB : leftCount > rightCount;
    }
    default: return QSortFilterProxyModel::lessThan(left, right);
    }
}

bool BusstopTableProxyModel::filterAcceptsRow(int sourceRow,
                                              const QModelIndex &sourceParent) const {
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}
