#include "RouteTableModel.h"

#include <QFont>

RouteTableModel::RouteTableModel(QObject *parent) : UnorderedProjectDataRowModel(parent), _line(nullptr) {}

Line *RouteTableModel::line() const {
    return _line;
}

void RouteTableModel::setLine(Line *line) {
    if(_line)
        _line->disconnect(this);

    _line = line;
    reset();
    connect(_line, &Line::routeAdded,   this, &RouteTableModel::onItemAdded);
    connect(_line, &Line::routeChanged, this, &RouteTableModel::onItemUpdated);
    connect(_line, &Line::routeRemoved, this, &RouteTableModel::onItemRemoved);
}

int RouteTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 8;
}

QVariant RouteTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(orientation != Qt::Horizontal
       || (role != Qt::DisplayRole
           && role != ColumnVisibleByDefaultRole
           && role != ColumnMandatoryRole))
        return QVariant();

    switch(role) {
    case ColumnVisibleByDefaultRole:
        return section <= 5;

    case ColumnMandatoryRole:
        return section == 2;

    case Qt::DisplayRole:
        switch(section) {
            case 0: return tr("Code");
            case 1: return tr("Direction");
            case 2: return tr("Name");
            case 3: return tr("First Busstop");
            case 4: return tr("Last Busstop");
            case 5: return tr("Stop Count");
            case 6: return tr("Profile Count");
            case 7: return tr("Duration");
        }
    }

    return QVariant();
}

QVariant RouteTableModel::data(const QModelIndex &index, int role) const {
    if(role != Qt::DisplayRole && role != Qt::FontRole)
        return QVariant();

    switch(role) {
    case Qt::FontRole:
        if(index.column() == 2) {
            QFont f;
            f.setBold(true);
            return f;
        } else {
            return QFont();
        }

    case Qt::DisplayRole:
        Route *route = itemAt(index.row());

        switch(index.column()) {
        case 0: return route->code();
        case 1: return route->direction()->description();
        case 2: return route->name();
        case 3: return route->busstopCount() > 0 ? route->firstBusstop()->busstop()->name() : "";
        case 4: return route->busstopCount() > 0 ? route->lastBusstop()->busstop()->name() : "";
        case 5: return route->busstopCount();
        case 6: return "";
        case 7: return "";
        }
    }

    return QVariant();
}

PDISet<Route> RouteTableModel::fetch() const {
    if(!_line)
        return {};

    return _line->routes();
}
