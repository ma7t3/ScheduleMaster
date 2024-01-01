#include "ProjectData\tour.h"

Tour::Tour(QString id, QString name, WeekDays weekDays) :
    AbstractProjectDataItem(id),
    _name(name),
    _weekDays(new WeekDays(weekDays))
{}

void Tour::setName(QString n) { _name = n; }
void Tour::setWeekDays(WeekDays w) { _weekDays = new WeekDays(w); }

QString Tour::name() { return _name; }
WeekDays * Tour::weekDays() { return _weekDays; }

int Tour::indexOfTrip(Trip *t) {
    for(int i = 0; i < _trips.count(); i++)
        if(_trips[i] == t)
            return i;

    return -1;
}

void Tour::addTrip(Trip *t) { _trips << t; }

void Tour::insertTripAt(Trip *t, int i) { _trips.insert(i, t); }

void Tour::insertTripAfter(Trip *lt, Trip *t) {
    for(int i = 0; i < _trips.count(); i++) {
        if(_trips[i]->id() == lt->id()) {
            _trips.insert(i + 1, t);
            return;
        }
    }

    _trips << t;
}

void Tour::setTripList(QList<Trip *> list) { _trips = list; }

void Tour::removeTrip(Trip *t)
{
    for(int i = 0; i < _trips.count(); i++) {
        if(_trips[i]->id() != t->id())
            continue;

        _trips.remove(i);
        return;
    }
}

int Tour::tripCount() { return _trips.count(); }
QList<Trip *> Tour::trips() { return _trips; }

Trip *Tour::trip(QString id) {
    for(int i = 0; i < tripCount(); i++)
        if(tripAt(i)->id() == id)
            return tripAt(i);

    return nullptr;
}

Trip * Tour::tripAt(int i)
{
    if(i < 0 || i >= _trips.count())
        return nullptr;

    return _trips[i];
}

bool Tour::hasTrip(Trip *t) {
    for(int i = 0; i < tripCount(); i++)
        if(tripAt(i) == t)
            return true;

    return false;
}

bool Tour::goesPastMidnight() {
    return startTime() > endTime();
}

bool Tour::tripIsAfterMidnight(Trip *t) {
    return t->startTime() < startTime();
}

bool Tour::tripIsAfterMidnight(QString id){
    Trip *t = trip(id);
    if(!t)
        return false;

    return tripIsAfterMidnight(t);
}

QTime Tour::startTime() {
    if(_trips.empty())
        return QTime(0, 0, 0, 0);

    return tripAt(0)->startTime();
}

QTime Tour::endTime() {
    if(_trips.empty())
        return QTime(0, 0, 0, 0);

    return tripAt(tripCount() - 1)->endTime();
}

QTime Tour::duration() {
    if(_trips.empty())
        return QTime(0, 0, 0, 0);

    QTime time(0, 0, 0, 0);
    int start = tripAt(0)->startTime().msecsSinceStartOfDay();
    int end = tripAt(tripCount() - 1)->endTime().msecsSinceStartOfDay();

    if(end < start)
        start += 86400000;

    return time.addMSecs(end - start);
}

QTime Tour::drivingTime() {
    QTime time(0, 0, 0, 0);
    for(int i = 0; i < tripCount(); i++)
        time = time.addMSecs(tripAt(i)->duration().msecsSinceStartOfDay());

    return time;
}

QTime Tour::breakTime() {
    QTime time(0, 0, 0, 0);
    return time.addMSecs(duration().msecsSinceStartOfDay() - drivingTime().msecsSinceStartOfDay());
}

void Tour::overwrite(Tour &other) {
    setName(other.name());
    setWeekDays(*other.weekDays());
    setTripList(other.trips());
}

bool Tour::operator<(Tour &other) {
    return name() < other.name();
}















