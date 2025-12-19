#include "BusstopTableProxyModel.h"

#include "src/projectdata/ProjectData.h"
#include "src/ui/widgets/FilterPopups/WdgBusstopsFilterPopup.h"

#include "ApplicationInterface.h"

BusstopTableProxyModel::BusstopTableProxyModel(QAbstractButton *popupButton, QObject *parent) :
    SortFilterProxyModel<WdgBusstopsFilterPopup>(popupButton, parent), _projectData(ApplicationInterface::projectData()) {}

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

    if(!QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent))
        return false;

    const BusstopFlags flags = filterPopup()->filterFlags();
    Busstop *b = static_cast<Busstop *>(sourceModel()->index(sourceRow, 0).internalPointer());
    PDISet<Line> linesAtBusstop = _projectData->linesAtBusstop(b);
    PDISet<Line> selectedLines  = filterPopup()->filterLines();

    const bool anyLineMatch = std::any_of(linesAtBusstop.begin(), selectedLines.end(), [selectedLines](Line *l) {
        return selectedLines.contains(l);
    }) || selectedLines.isEmpty();

    return ((flags == BusstopFlag::StandardBusstop) || (b->flags() & flags)) && anyLineMatch;
}

