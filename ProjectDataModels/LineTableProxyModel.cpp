#include "LineTableProxyModel.h"

#include "ProjectData/Line.h"

#include <QCollator>

LineTableProxyModel::LineTableProxyModel(QObject *parent) : QSortFilterProxyModel{parent} {}

bool LineTableProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    QString lineNameA = static_cast<Line *>(left.internalPointer())->name();
    QString lineNameB = static_cast<Line *>(right.internalPointer())->name();

    static thread_local QCollator collator;
    collator.setNumericMode(true);
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    return collator.compare(lineNameA, lineNameB) < 0;
}

bool LineTableProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    QModelIndex nameIndex        = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex descriptionIndex = sourceModel()->index(sourceRow, 1, sourceParent);
    QString name        = nameIndex.data(Qt::DisplayRole).toString();
    QString description = descriptionIndex.data(Qt::DisplayRole).toString();

    const QString filter = filterRegularExpression().pattern();
    return name.contains(filter, filterCaseSensitivity()) || description.contains(filter, filterCaseSensitivity());
}
