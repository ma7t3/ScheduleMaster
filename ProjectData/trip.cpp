#include "ProjectData\trip.h"
#include "ProjectData\timeProfile.h"
#include "App/global.h"

Trip::Trip(QString id, Route *route, QTime startTime, TimeProfile *timeProfile, WeekDays weekDays) :
    ProjectDataItem(id),
    _route(route),
    _startTime(startTime),
    _weekDays(new WeekDays(weekDays)),
    _timeProfile(timeProfile)
{}

void Trip::setRoute(Route *r) {
    _route = r;
}

void Trip::setStartTime(QTime t) {
    _startTime = t;
}

void Trip::setTimeProfile(TimeProfile *p) {
    _timeProfile = p;
}

Route *Trip::route()    { return _route; }
QTime Trip::startTime() { return _startTime; }

QTime Trip::endTime() {
    QTime d = duration();
    return _startTime.addMSecs(d.msecsSinceStartOfDay());
}

QTime Trip::duration() {
    float durMinutes = _timeProfile->duration();
    QTime t(0, 0, 0, 0);
    return t.addSecs(durMinutes * 60);
}

QTime Trip::busstopTime(Busstop *b) {
    TimeProfileItem *itm = _timeProfile->busstop(b);
    if(!itm)
        return QTime(0, 0, 0, 0);
    
    return _startTime.addSecs(itm->depValue() * 60);
}

QTime Trip::busstopTime(QString id) {
    TimeProfileItem *itm = _timeProfile->busstop(id);
    if(!itm)
        return QTime(0, 0, 0, 0);
    
    return _startTime.addSecs(itm->depValue() * 60);
}

WeekDays * Trip::weekDays()       { return _weekDays; }
TimeProfile * Trip::timeProfile() { return _timeProfile; }

bool Trip::goesPastMidnight() {
    return _startTime > endTime();
}

bool Trip::busstopIsAfterMidnight(Busstop *b) {
    if(!route()->hasBusstop(b))
        return false;
    
    return _startTime > busstopTime(b);
}

bool Trip::busstopIsAfterMidnight(QString id) {
    if(!route()->hasBusstop(id))
        return false;
    
    return _startTime > busstopTime(id);
}

void Trip::overwrite(Trip &other) {
    setRoute(other.route());
    setStartTime(other.startTime());
    *weekDays() = *other.weekDays();
    setTimeProfile(other.timeProfile());
}

bool Trip::operator<(Trip &other) {
    return startTime() < other.startTime();
}
