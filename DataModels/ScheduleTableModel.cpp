#include "ScheduleTableModel.h"

#include <QFont>

#include "localconfig.h"

ScheduleTableModel::ScheduleTableModel(QObject *parent) : OrderedProjectDataColumnModel(parent),
    _projectData(nullptr), _line(nullptr), _direction(nullptr), _showOnlyImportant(false) {
    _dataFieldsCount = HeaderRowCount;
}

void ScheduleTableModel::setLine(Line *newLine) {
    if(_line)
        _line->disconnect(this);

    _line = newLine;

    if(_line) {
        _projectData = dynamic_cast<ProjectData *>(_line->parent());

        connect(_line, &Line::tripsAdded,       this, &ScheduleTableModel::addItems);
        connect(_line, &Line::tripsChanged,     this, &ScheduleTableModel::changeItems);
        connect(_line, &Line::tripsRemoved,     this, &ScheduleTableModel::removeItems);

        connect(_line, &Line::tripsAdded,       this, &ScheduleTableModel::recalculateDataFieldsCount);
        connect(_line, &Line::tripsChanged,     this, &ScheduleTableModel::recalculateDataFieldsCount);
        connect(_line, &Line::tripsRemoved,     this, &ScheduleTableModel::recalculateDataFieldsCount);

        connect(_line, &Line::tripsAdded,       this, &ScheduleTableModel::reloadCache);
        connect(_line, &Line::tripsChanged,     this, &ScheduleTableModel::reloadCache);
        connect(_line, &Line::tripsRemoved,     this, &ScheduleTableModel::reloadCache);

        connect(_line, &Line::hourBreakChanged, this, &ScheduleTableModel::reset);
    }
    reset();
}

void ScheduleTableModel::setDirection(LineDirection *newDirection) {
    _direction = newDirection;
    reset();
}

void ScheduleTableModel::setWeekDays(WeekDays newWeekDays) {
    _weekDays = newWeekDays;
    reset();
}

void ScheduleTableModel::setAll(Line *newLine, LineDirection *newDirection, WeekDays newWeekDays) {
    setLine(newLine);
    setDirection(newDirection);
    setWeekDays(newWeekDays);
}

QVariant ScheduleTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if((role != Qt::DisplayRole && role != Qt::FontRole && role != 0x0100) || orientation == Qt::Horizontal)
        return QVariant();

    if(role == Qt::DisplayRole) {
        switch(section) {
            case 0: return tr("Tour");
            case 1: return tr("Info");
            case 2: return tr("Footnotes");
            case 3: return tr("Route Name");
            case 4: return tr("Time Profile");
            case 5: return tr("Start Time");
            case 6: return tr("Busstop List");
        }
    }

    int busstopIndex = section - HeaderRowCount;

    if(busstopIndex >= _busstopList.count() || busstopIndex < 0)
        return QVariant();

    switch(role) {
    case 0x0100:
        return _busstopList[busstopIndex]->isImportant();
    case Qt::DisplayRole:
        return _busstopList[busstopIndex]->name();

    case Qt::FontRole:
        QFont bold;
        if(_busstopList[busstopIndex]->isImportant())
            bold.setBold(true);
        return bold;    
    }

    return QVariant();
}

int ScheduleTableModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return _busstopList.length() + HeaderRowCount;
}

QVariant ScheduleTableModel::data(const QModelIndex &index, int role) const {
    // skip if indeex is invalid
    if (!index.isValid())
        return QVariant();

    // always align everything centered
    if(role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;


    Trip    *t = _items[index.column()];
    Busstop *b = busstopAt(index.row());

    bool header = index.row() < HeaderRowCount;

    switch(role) {
        case Qt::DisplayRole:

        if(header) {
            QStringList dataList;
            switch(index.row()) {
            case 0:
                for(Tour *o : _tripCache[t].tours)
                    dataList << o->name();
                if(dataList.isEmpty())
                    return tr("None");
                else
                    return dataList.join(",");

            case 1:
                return QVariant();

            case 2:
                for(Footnote *f : _tripCache[t].footnotes)
                    dataList << f->identifier();
                return dataList.join(", ");

            case 3:
                return t->route()->name();
            case 4:
                return t->timeProfile()->name();
            case 5:
                return t->startTime().toString(LocalConfig::timeFormatString(false, false));
            }
        } else {
            if(t->route()->hasBusstop(b)) {
                QTime time = t->busstopTime(b);
                if(time.isValid())
                    return time.toString(LocalConfig::timeFormatString(false, false));
                else
                    return "--:--";
            } else {
                if(
                    t->route()->busstopCount() == 0 ||
                    _busstopList.indexOf(t->route()->firstBusstop()) > index.row() - HeaderRowCount ||
                    _busstopList.indexOf(t->route()->lastBusstop()) < index.row() - HeaderRowCount
                    ) {
                    return QVariant();
                } else {
                    return "|";
                }
            }
        }

        case Qt::ToolTipRole:
            if(index.row() == 0) {
                QList<Tour *> tours = _tripCache[t].tours;
                if(tours.isEmpty())
                    return tr("<i>This trip has no tours assigned</i>");

                QString tourStr = "<html><body>";
                for(Tour *o : tours) {
                    tourStr += "<tr><td><b>" + o->name() + "</b></td><td>" + o->weekDays().toString() + "</td></tr>";
                }
                return tourStr += "</body></html>";
            } else if(index.row() == 2) {
                QList<Footnote *> footnotes = _tripCache[t].footnotes;
                if(footnotes.isEmpty())
                    return QVariant();

                QString footnotesStr = "<html><body>";
                for(Footnote *f : footnotes) {
                    footnotesStr += "<tr><td><b>" + f->identifier() + ": </b></td><td>" + f->description() + "</td></tr>";
                }
                return footnotesStr += "</body></html>";
            } else if(index.row() == 3) {
                Route *r = t->route();
                return "<b>" + r->name() + "</b> (" + (r->code() < 10 ? "0" : "") + QString::number(r->code()) + ")";
            } else if(index.row() == 4) {
                TimeProfile *p = t->timeProfile();
                return "<b>" + p->name() + "</b> (" + QString::number(p->duration()) + " min.)";
            }
            break;

        case Qt::FontRole:
            if(index.row() == 5 || (index.row() >= HeaderRowCount && b->isImportant() && t->route()->hasBusstop(b))) {
                QFont f;
                f.setBold(true);
                return f;
            } else {
                return QVariant();
            }

        case Qt::ForegroundRole:
            if(header) {
                switch(index.row()) {
                case 0:
                    switch(_tripCache[t].tourStatusType) {
                    case TripCache::None:    return QColor(Qt::yellow);
                    case TripCache::Match:   case TripCache::TooFew: return QColor(Qt::white);
                    case TripCache::TooMuch: return QColor(Qt::black);
                    }
                case 2:
                    return QColor(Qt::black);
                case 4:
                case 5:
                    return QColor(Qt::white);
                }
            } else
                return QVariant();
            break;

        case Qt::BackgroundRole:
        if(header) {
            switch(index.row()) {
            case 0:
                switch(_tripCache[t].tourStatusType) {
                    case TripCache::None:    return QColor(Qt::red);
                    case TripCache::Match:   return QColor(Qt::darkGreen);
                    case TripCache::TooFew:  return QColor(Qt::blue);
                    case TripCache::TooMuch: return QColor(255, 168, 0);
                }

            case 2:
                if(_tripCache[t].footnotes.isEmpty())
                    return QVariant();
                else
                    return QColor(Qt::yellow);
            case 4:
                return profileColors[qMin(t->route()->indexOfTimeProfile(t->timeProfile()), 4)];
            case 5:
                return QColor(85, 85, 85);
            }
        }
    }

    return QVariant();
}

Busstop *ScheduleTableModel::busstopAt(const int &index) const {
    if(index < 0 || index < HeaderRowCount || index - HeaderRowCount >= _busstopList.count())
        return nullptr;

    return _busstopList[index - HeaderRowCount];
}

void ScheduleTableModel::setShowOnlyImportant(const bool &b) {
    _showOnlyImportant = b;
}

QList<Trip *> ScheduleTableModel::fetchData() const {
    if(!_projectData || !_line || !_direction)
        return {};

    QList<Trip *> allTrips = _line->tripsToDirection(_direction);
    QList<Trip *> filteredTrips;
    for(Trip *t : allTrips) {
        if(WeekDays::overlap(t->weekDays(), _weekDays))
            filteredTrips << t;
    }

    return ProjectData::sortTrips(filteredTrips, _line->hourBreak());
}

void ScheduleTableModel::reset() {
    _updating = true;
    QAbstractTableModel::beginResetModel();
    _items.clear();
    _items = fetchData();
    _busstopList = createBusstopList(_items);
    recalculateDataFieldsCount();
    reloadCache();
    QAbstractTableModel::endResetModel();
    _updating = false;
    emit ProjectDataModelSignals::updateFinished();
}

void ScheduleTableModel::reloadCache() {
    _tripCache.clear();

    for(Trip *t : _items) {

        TripCache c;

        QList<Tour *> tours = ProjectData::sortItems(_projectData->toursOfTrip(t));
        c.tours = tours;

        QList<Footnote *> footnotes = ProjectData::sortItems(_projectData->autoAssignedFootnotesOfTrip(t));
        c.footnotes = footnotes;

        if(tours.isEmpty()) {
            c.tourStatusType = TripCache::None;
            _tripCache.insert(t, c);
            continue;
        }

        QList<WeekDays> tourWeekDays;
        for(Tour *t : tours)
            tourWeekDays << t->weekDays();


        WeekDays combinedTourWeekDays = WeekDays::combine(tourWeekDays);
        bool match = combinedTourWeekDays == t->weekDays();
        bool overlap = WeekDays::overlap(tourWeekDays);

        if(match && !overlap)
            c.tourStatusType = TripCache::Match;
        else if(overlap)
            c.tourStatusType = TripCache::TooMuch;
        else
            c.tourStatusType = TripCache::TooFew;

        _tripCache.insert(t, c);
    }
}

QList<Busstop *> ScheduleTableModel::createBusstopList(QList<Trip *> trips) {
    QList<Busstop *> result;

    QList<Route *> routes;
    for(int i = 0; i < trips.count(); i++) {
        Route *r = trips[i]->route();
        if(!routes.contains(r))
            routes << r;
    }

    routes = ProjectData::sortItems(routes);

    for(Route *r : routes) {
        Busstop *lastBusstop = nullptr;

        // for each busstop
        for(int j = 0; j < r->busstopCount(); j++) {
            Busstop *b = r->busstopAt(j);

            // skip if not important, but only important busstops should be shown
            if(_showOnlyImportant && !b->isImportant())
                continue;

            // determine, if busstop was already added
            if(result.contains(b)) {
                lastBusstop = b;
                continue;
            }

            int targetIndex;
            if(lastBusstop)
                targetIndex = result.indexOf(lastBusstop) + 1;
            else
                targetIndex = 0;

            result.insert(targetIndex, b);
            lastBusstop = b;
        }
    }

    return result;
}

void ScheduleTableModel::recalculateDataFieldsCount() {
    _dataFieldsCount = _busstopList.count() + HeaderRowCount;
}
