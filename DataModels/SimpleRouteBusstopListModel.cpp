#include "SimpleRouteBusstopListModel.h"

#include <QFont>

SimpleRouteBusstopListModel::SimpleRouteBusstopListModel(QObject *parent) : OrderedProjectDataRowModel(parent) {}

void SimpleRouteBusstopListModel::setRoute(Route *newRoute) {
    _route = newRoute;
    connect(_route, &Route::busstopsAdded,   this, &SimpleRouteBusstopListModel::addItems);
    connect(_route, &Route::busstopsChanged, this, &SimpleRouteBusstopListModel::changeItems);
    connect(_route, &Route::busstopsRemoved, this, &SimpleRouteBusstopListModel::removeItems);
    reset();
}

QVariant SimpleRouteBusstopListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    Busstop *b = _items[index.row()];

    switch(role) {
    case Qt::DisplayRole:
        return b->name();

    case Qt::FontRole:
        QFont f;
        if(b->isImportant())
            f.setBold(true);
        return f;
    }

    return QVariant();
}

QList<Busstop *> SimpleRouteBusstopListModel::fetchData() const {
    return _route->busstops();
}
