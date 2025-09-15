#include "LineTableProxyModel.h"

#include "ProjectData/Line.h"

#include <QCollator>

LineTableProxyModel::LineTableProxyModel(QObject *parent) : QSortFilterProxyModel{parent} {}

bool LineTableProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    Line *a = static_cast<Line *>(left.internalPointer());
    Line *b = static_cast<Line *>(right.internalPointer());
    return *a < *b;
}

bool LineTableProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    QModelIndex nameIndex        = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex descriptionIndex = sourceModel()->index(sourceRow, 1, sourceParent);
    QString name        = nameIndex.data(Qt::DisplayRole).toString();
    QString description = descriptionIndex.data(Qt::DisplayRole).toString();

    const QRegularExpression expr = filterRegularExpression();

    return expr.match(name).hasMatch() || expr.match(description).hasMatch();
}
