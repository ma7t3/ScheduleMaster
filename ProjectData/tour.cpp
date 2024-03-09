#include "ProjectData\tour.h"
#include "projectdata.h"

Tour::Tour(QObject *parent, const QString &id, const QString &name, const WeekDays &weekDays) :
    ProjectDataItem(parent, id), _name(name), _weekDays(new WeekDays(weekDays)) {
}

Tour::Tour(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent),
    _weekDays(new WeekDays(this)) {
    fromJson(jsonObject);
}

Tour::Tour(const Tour &other) :
    ProjectDataItem(other.parent()) {
    copy(other);
}

Tour Tour::operator=(const Tour &other) {
    copy(other);
    return *this;
}

bool Tour::operator<(Tour &other) {
    return name() < other.name();
}

void Tour::copy(const Tour &other) {
    ProjectDataItem::copy(other);
    setName(other.name());
    setWeekDays(*other.weekDays());
    setTrips(other.trips());
}

void Tour::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);

    setName(jsonObject.value("name").toString(tr("unnamed tour")));
    weekDays()->setCode(jsonObject.value("weekdays").toInt());

    QJsonArray jTrips = jsonObject.value("trips").toArray();
    for (int i = 0; i < jTrips.count(); ++i)
        if(jTrips[i].isString())
            addTrip(static_cast<ProjectData *>(parent())->trip(jTrips[i].toString()));
}

QJsonObject Tour::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("name", name());
    jsonObject.insert("weekdays", weekDays()->toCode());

    QJsonArray jTrips;
    for (int i = 0; i < tripCount(); ++i)
        jTrips << tripAt(i)->id();

    jsonObject.insert("trips", jTrips);

    return jsonObject;
}

void Tour::refreshChilds() {
    _weekDays->setParent(this);
}

void Tour::setName(const QString &newName) {
    _name = newName;
}

void Tour::setWeekDays(const WeekDays &newWeekDays) {
    _weekDays = new WeekDays(newWeekDays);
}

QString Tour::name() const {
    return _name;
}

WeekDays *Tour::weekDays() const {
    return _weekDays;
}

int Tour::indexOfTrip(Trip *t) const {
    for(int i = 0; i < _trips.count(); i++)
        if(_trips[i] == t)
            return i;

    return -1;
}

void Tour::addTrip(Trip *t) {
    if(!t)
        return;

    _trips << t;
}

void Tour::insertTripAt(Trip *t, const int &index) {
    _trips.insert(index, t);
}

void Tour::insertTripAfter(Trip *lt, Trip *t) {
    for(int i = 0; i < _trips.count(); i++) {
        if(_trips[i]->id() == lt->id()) {
            _trips.insert(i + 1, t);
            return;
        }
    }

    _trips << t;
}

void Tour::setTrips(const QList<Trip *> &newTrips) {
    _trips = newTrips;
}

void Tour::removeTrip(Trip *t)
{
    for(int i = 0; i < _trips.count(); i++) {
        if(_trips[i]->id() != t->id())
            continue;

        _trips.remove(i);
        return;
    }
}

int Tour::tripCount() const {
    return _trips.count();
}

QList<Trip *> Tour::trips() const {
    return _trips;
}

Trip *Tour::trip(const QString &id) const {
    for(int i = 0; i < tripCount(); i++)
        if(tripAt(i)->id() == id)
            return tripAt(i);

    return nullptr;
}

Trip * Tour::tripAt(const int &index) const {
    if(index < 0 || index >= _trips.count())
        return nullptr;

    return _trips[index];
}

bool Tour::hasTrip(Trip *t) const {
    for(int i = 0; i < tripCount(); i++)
        if(tripAt(i) == t)
            return true;

    return false;
}

bool Tour::goesPastMidnight() const {
    return startTime() > endTime();
}

bool Tour::tripIsAfterMidnight(Trip *t) const {
    return t->startTime() < startTime();
}

bool Tour::tripIsAfterMidnight(const QString &id) const {
    Trip *t = trip(id);
    if(!t)
        return false;

    return tripIsAfterMidnight(t);
}

QTime Tour::startTime() const {
    if(_trips.empty())
        return QTime(0, 0, 0, 0);

    return tripAt(0)->startTime();
}

QTime Tour::endTime() const {
    if(_trips.empty())
        return QTime(0, 0, 0, 0);

    return tripAt(tripCount() - 1)->endTime();
}

QTime Tour::duration() const {
    if(_trips.empty())
        return QTime(0, 0, 0, 0);

    QTime time(0, 0, 0, 0);
    int start = tripAt(0)->startTime().msecsSinceStartOfDay();
    int end = tripAt(tripCount() - 1)->endTime().msecsSinceStartOfDay();

    if(end < start)
        start += 86400000;

    return time.addMSecs(end - start);
}

QTime Tour::drivingTime() const {
    QTime time(0, 0, 0, 0);
    for(int i = 0; i < tripCount(); i++)
        time = time.addMSecs(tripAt(i)->duration().msecsSinceStartOfDay());

    return time;
}

QTime Tour::breakTime() const {
    QTime time(0, 0, 0, 0);
    return time.addMSecs(duration().msecsSinceStartOfDay() - drivingTime().msecsSinceStartOfDay());
}
