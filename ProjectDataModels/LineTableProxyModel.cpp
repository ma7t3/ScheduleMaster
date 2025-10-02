#include "LineTableProxyModel.h"

#include "ProjectData/Line.h"

#include <QCollator>
#include <QWidget>

LineTableProxyModel::LineTableProxyModel(QObject *parent) : SortFilterProxyModel<WdgBusstopsFilterPopup>(nullptr, parent) {}

bool LineTableProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    switch(sortColumn()) {
    case 0: {
        Line *a = static_cast<Line *>(left.internalPointer());
        Line *b = static_cast<Line *>(right.internalPointer());
        return *a < *b;
    }
    default:
        return QSortFilterProxyModel::lessThan(left, right);
    }
}

bool LineTableProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    QModelIndex nameIndex        = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex descriptionIndex = sourceModel()->index(sourceRow, 1, sourceParent);
    QString name        = nameIndex.data(Qt::DisplayRole).toString();
    QString description = descriptionIndex.data(Qt::DisplayRole).toString();

    const QRegularExpression expr = filterRegularExpression();

    return expr.match(name).hasMatch() || expr.match(description).hasMatch();
}
