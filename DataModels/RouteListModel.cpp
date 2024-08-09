#include "RouteListModel.h"

RouteListModel::RouteListModel(QObject *parent) :
    UnorderedProjectDataRowModel(parent),
    _line(nullptr),
    _direction(nullptr) {}

void RouteListModel::setDirection(LineDirection *newLineDirection) {
    if(_line)
        _line->disconnect(this);

    if(!newLineDirection) {
        _line      = nullptr;
        _direction = nullptr;
    } else {
        _direction = newLineDirection;
        _line      = dynamic_cast<Line *>(_direction->parent());
    }

    reset();
}

QVariant RouteListModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid())
        return QVariant();

    Route *r = _items[index.row()];

    switch(role) {
    case Qt::DisplayRole:
        return r->name();

    case Qt::ToolTipRole:
        return (r->timeProfileCount() == 0 ? tr("This route has no time profiles and can not be used.") : QVariant());
    }

    return QVariant();
}

Qt::ItemFlags RouteListModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if(_items[index.row()]->timeProfileCount() == 0) {
        flags &= ~Qt::ItemIsEnabled;
    }
    return flags;
}

QList<Route *> RouteListModel::fetchData() const {
    if(!_line || !_direction)
        return {};

    return ProjectData::sortItems(_line->routesToDirection(_direction));
}
