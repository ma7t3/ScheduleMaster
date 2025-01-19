#include "TimeProfileTableModel.h"

TimeProfileTableModel::TimeProfileTableModel(QObject *parent) : OrderedProjectDataRowModel(parent) {
    _dataFieldsCount = 2;
}

void TimeProfileTableModel::setRoute(Route *newRoute) {
    _route = newRoute;
    connect(_route, &Route::timeProfilesAdded,   this, &TimeProfileTableModel::addItems);
    connect(_route, &Route::timeProfilesChanged, this, &TimeProfileTableModel::changeItems);
    connect(_route, &Route::timeProfilesRemoved, this, &TimeProfileTableModel::removeItems);
    reset();
}

QVariant TimeProfileTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole || orientation == Qt::Vertical)
        return QVariant();

    switch(section) {
    case 0:
        return tr("Name");
    case 1:
        return tr("Duration");
    }

    return QVariant();
}

int TimeProfileTableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return _dataFieldsCount;
}

QVariant TimeProfileTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    TimeProfile *p = _items[index.row()];

    switch(index.column()) {
    case 0:
        switch(role) {
        case Qt::DisplayRole:
            return p->name();
        } break;
    case 1:
        switch(role) {
        case Qt::DisplayRole:
            return QString::number(p->duration()) + tr(" min.");
        }
    }

    return QVariant();
}

QList<TimeProfile *> TimeProfileTableModel::fetchData() const {
    return _route->timeProfiles();
}
