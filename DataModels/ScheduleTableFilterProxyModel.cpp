#include <QGuiApplication>
#include <QStyleHints>
#include "ScheduleTableFilterProxyModel.h"
#include "DataModels/ScheduleTableModel.h"

ScheduleTableFilterProxyModel::ScheduleTableFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    _onlyImportant(false) {

}

void ScheduleTableFilterProxyModel::setBusstopNameFilter(const QString &text) {
    _nameFilter = text;
    invalidateFilter();
}

void ScheduleTableFilterProxyModel::setOnlyImportantBusstop(const bool &b) {
    _onlyImportant = b;
    invalidateFilter();
}

QVariant ScheduleTableFilterProxyModel::data(const QModelIndex &index, int role) const {
    if(index.row() < ScheduleTableModel::HeaderRowCount || role != Qt::BackgroundRole)
        return QSortFilterProxyModel::data(index, role);

    // TODO: This is Pfusch!
    if(index.row() % 2 == 1) {
        if(qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
            return QColor(31, 31, 31);
        else
            return QColor(224, 224, 224);
    }
    return QVariant();
}

bool ScheduleTableFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    Q_UNUSED(sourceParent);

    if(sourceRow < ScheduleTableModel::HeaderRowCount)
        return true;

    QAbstractItemModel *sourceModel = this->sourceModel();

    if(_onlyImportant && !sourceModel->headerData(sourceRow, Qt::Vertical, 0x0100).toBool())
        return false;

    QString headerText = sourceModel->headerData(sourceRow, Qt::Vertical, Qt::DisplayRole).toString();
    return headerText.contains(_nameFilter, Qt::CaseInsensitive);
}
