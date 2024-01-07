#include "trip.h"
#include "timeProfile.h"

Trip::Trip(QString id, Route *route, QTime startTime, TimeProfile *timeProfile, WeekDays weekDays) :
    ProjectDataItem(id), _route(route), _startTime(startTime), _weekDays(new WeekDays(weekDays)),
    _timeProfile(timeProfile) {
}

Trip::Trip(const Trip &other) {
    copy(other);
}

bool Trip::operator<(const Trip &other) {
    return startTime() < other.startTime();
}

Trip Trip::operator=(const Trip &other) {
    copy(other);
    return *this;
}

void Trip::copy(const Trip &other) {
    ProjectDataItem::copy(other);
    setRoute(other.route());
    setStartTime(other.startTime());
    *weekDays() = *other.weekDays();
    setTimeProfile(other.timeProfile());
}

Route *Trip::route() const {
    return _route;
}

void Trip::setRoute(Route *r) {
    _route = r;
}

QTime Trip::startTime() const {
    return _startTime;
}

void Trip::setStartTime(const QTime &newStartTime) {
    _startTime = newStartTime;
}

QTime Trip::endTime() const {
    QTime d = duration();
    return _startTime.addMSecs(d.msecsSinceStartOfDay());
}

QTime Trip::duration() const {
    float durMinutes = _timeProfile->duration();
    QTime t(0, 0, 0, 0);
    return t.addSecs(durMinutes * 60);
}

QTime Trip::busstopTime(Busstop *b) const {
    TimeProfileItem *itm = _timeProfile->busstop(b);
    if(!itm)
        return QTime(0, 0, 0, 0);
    
    return _startTime.addSecs(itm->depValue() * 60);
}

QTime Trip::busstopTime(const QString &id) const {
    TimeProfileItem *itm = _timeProfile->busstop(id);
    if(!itm)
        return QTime(0, 0, 0, 0);
    
    return _startTime.addSecs(itm->depValue() * 60);
}

TimeProfile * Trip::timeProfile() const {
    return _timeProfile;
}

void Trip::setTimeProfile(TimeProfile *p) {
    _timeProfile = p;
}

WeekDays * Trip::weekDays() const {
    return _weekDays;
}

bool Trip::goesPastMidnight() const {
    return _startTime > endTime();
}

bool Trip::busstopIsAfterMidnight(Busstop *b) const {
    if(!route()->hasBusstop(b))
        return false;
    
    return _startTime > busstopTime(b);
}

bool Trip::busstopIsAfterMidnight(const QString &id) const {
    if(!route()->hasBusstop(id))
        return false;
    
    return _startTime > busstopTime(id);
}


