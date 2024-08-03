#include "RouteTableModel.h"

#include <QFont>

RouteTableModel::RouteTableModel(QObject *parent) : UnorderedProjectDataRowModel<Route>(parent), _line(nullptr), _refreshSorting(false) {
    _dataFieldsCount = 6;
}

void RouteTableModel::setLine(Line *l) {
    if(_line)
        _line->disconnect(this);

    _line = l;

    if(_line) {
        connect(_line, &Line::routesAdded,   this, &RouteTableModel::addItems);
        connect(_line, &Line::routesChanged, this, &RouteTableModel::changeItems);
        connect(_line, &Line::routesRemoved, this, &RouteTableModel::removeItems);

        connect(this, &QAbstractTableModel::rowsInserted, this, &RouteTableModel::refreshDefaultSortIndexes);
        connect(this, &QAbstractTableModel::dataChanged,  this, &RouteTableModel::refreshDefaultSortIndexes);
        connect(this, &QAbstractTableModel::rowsRemoved,  this, &RouteTableModel::refreshDefaultSortIndexes);
        connect(this, &QAbstractTableModel::modelReset,   this, &RouteTableModel::refreshDefaultSortIndexes);
    }

    reset();
}

QVariant RouteTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole || orientation == Qt::Vertical)
        return QVariant();

    switch(section) {
        case 0: return tr("Code");
        case 1: return tr("Direction");
        case 2: return tr("Name");
        case 3: return tr("First Busstop");
        case 4: return tr("Last Busstop");
        case 5: return tr("Busstop Count");
    }

    return QVariant();
}

int RouteTableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return _dataFieldsCount;
}

QVariant RouteTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    Route *r = _items[index.row()];

    switch(index.column()) {
        case 0:
            switch(role) {
            case 0x0100:
            case Qt::DisplayRole:
                return r->code();
            } break;

        case 1:
            switch(role) {
            case Qt::DisplayRole:
                return r->direction()->description();
            case 0x0100:
                return defaultSortIndexes[r];
            } break;

        case 2:
            switch(role) {
            case 0x0100:
            case Qt::DisplayRole:
                return r->name();

            case Qt::FontRole:
                QFont f;
                f.setBold(true);
                return f;
            } break;

        case 3:
            switch(role) {
                case 0x0100:
            case Qt::DisplayRole:
                if(r->busstopCount() != 0)
                    return r->firstBusstop()->name();
                else
                    return QVariant();
            } break;

        case 4:
            switch(role) {
            case 0x0100:
            case Qt::DisplayRole:
                if(r->busstopCount() != 0)
                    return r->lastBusstop()->name();
                else
                    return QVariant();
            } break;

        case 5:
            switch(role) {
            case 0x0100:
            case Qt::DisplayRole:
                return r->busstopCount();
            } break;
    }

    return QVariant();
}

QList<Route *> RouteTableModel::fetchData() const {
    if(!_line) return {};
    return ProjectData::sortItems(_line->routes());
}

void RouteTableModel::refreshDefaultSortIndexes() {
    if(_refreshSorting)
        return;

    _refreshSorting = true;
    defaultSortIndexes.clear();

    QList<Route *> sortedList = _items;
    std::sort(sortedList.begin(), sortedList.end(), [this](Route *a, Route *b){
        if(a->direction() == b->direction())
            return a->code() < b->code();

        const int indexA = _line->indexOfDirection(a->direction());
        const int indexB = _line->indexOfDirection(b->direction());
        return indexA < indexB;
    });

    for(int i = 0; i < sortedList.count(); i++) {
        Route *r = sortedList[i];
        defaultSortIndexes.insert(r, i);
    }

    // emit data changed to trigger re-sort by proxy models
    emit dataChanged(index(0, 1), index(_items.count() - 1, 1), {0x0100});

    _refreshSorting = false;
}
