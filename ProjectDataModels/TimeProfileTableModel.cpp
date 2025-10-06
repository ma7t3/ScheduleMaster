#include "TimeProfileTableModel.h"

#include <QLocale>

TimeProfileTableModel::TimeProfileTableModel(QObject *parent) :
    OrderedProjectDataRowModel(parent), _route(nullptr) {}

void TimeProfileTableModel::setRoute(Route *route) {
    if(_route)
        _route->disconnect(this);

    _route = route;
    reset();
    connect(_route, &Route::timeProfileAdded,   this, &TimeProfileTableModel::onItemInserted);
    connect(_route, &Route::timeProfileRemoved, this, &TimeProfileTableModel::onItemRemoved);
    connect(_route, &Route::timeProfileChanged, this, &TimeProfileTableModel::onItemUpdated);
    connect(_route, &Route::timeProfileMoved,   this, &TimeProfileTableModel::onItemMoved);
   }

int TimeProfileTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 2;
}

QVariant TimeProfileTableModel::headerData(int section, Qt::Orientation orientation,
                                           int role) const {
    if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch(section) {
    case 0:
        return tr("Name");

    case 1:
        return tr("Duration");
    }

    return QVariant();
}

QVariant TimeProfileTableModel::data(const QModelIndex &index, int role) const {
    if(role != Qt::DisplayRole)
        return QVariant();

    TimeProfile *p = itemAt(index.row());

    switch(index.column()) {
        case 0:
            return p->name();

        case 1: {
            QLocale locale;
            double minutes = p->duration();

            double rounded = std::round(minutes * 10.0) / 10.0;

            if (qFuzzyCompare(rounded, std::floor(rounded))) {
                return tr("%1 min").arg(locale.toString((int)rounded));
            } else {
                return tr("%1 min").arg(locale.toString(rounded, 'f', 1));
            }
        }
    }

    return QVariant();
}

PDIList<TimeProfile> TimeProfileTableModel::fetch() const {
    return _route ? _route->timeProfiles() : PDIList<TimeProfile>();
}
