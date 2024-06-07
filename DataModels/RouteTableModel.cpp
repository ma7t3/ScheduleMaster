#include "RouteTableModel.h"

#include <QFont>

RouteTableModel::RouteTableModel(QObject *parent) : UnorderedProjectDataRowModel<Route>(parent), _line(nullptr) {}

void RouteTableModel::setLine(Line *l) {
    if(_line)
        _line->disconnect(this);

    _line = l;

    if(_line) {
        connect(_line, &Line::routesAdded,   this, &RouteTableModel::addItems);
        connect(_line, &Line::routesChanged, this, &RouteTableModel::changeItems);
        connect(_line, &Line::routesRemoved, this, &RouteTableModel::removeItems);
    }

    reset();
}

QVariant RouteTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole || orientation == Qt::Vertical)
        return QVariant();

    switch(section) {
        case 0: return tr("Code");
        case 1: return tr("Direction");
        case 2: return tr("Nane");
        case 3: return tr("First Busstop");
        case 4: return tr("Last Busstop");
        case 5: return tr("Busstop Count");
    }

    return QVariant();
}

int RouteTableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return 6;
}

QVariant RouteTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    Route *r = _items[index.row()];

    switch(index.column()) {
        case 0:
            switch(role) {
            case Qt::DisplayRole:
                return QString::number(r->code());
            } break;

        case 1:
            switch(role) {
            case Qt::DisplayRole:
                return r->direction()->description();
            } break;

        case 2:
            switch(role) {
            case Qt::DisplayRole:
                return r->name();

            case Qt::FontRole:
                QFont f;
                f.setBold(true);
                return f;
            } break;

        case 3:
            switch(role) {
            case Qt::DisplayRole:
                if(r->busstopCount() != 0)
                    return r->firstBusstop()->name();
                else
                    return QVariant();
            } break;

        case 4:
            switch(role) {
            case Qt::DisplayRole:
                if(r->busstopCount() != 0)
                    return r->lastBusstop()->name();
                else
                    return QVariant();
            } break;

        case 5:
            switch(role) {
            case Qt::DisplayRole:
                return QString::number(r->busstopCount());
            } break;
    }

    return QVariant();
}

QList<Route *> RouteTableModel::fetchData() const {
    if(!_line) return {};
    return ProjectData::sortItems(_line->routes());
}

bool RouteTableModel::testSearchMatch(Route *r) const {
    return r->name().contains(_search, Qt::CaseInsensitive);
}
