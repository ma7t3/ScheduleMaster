#include "ProjectData\trip.h"
#include "ProjectData\timeProfile.h"
#include "App/global.h"

Trip::Trip(QString id, Route *route, QTime startTime, TimeProfile *timeProfile, WeekDays weekDays) :
    AbstractProjectDataItem(id),
    m_route(route),
    m_startTime(startTime),
    m_weekDays(new WeekDays(weekDays)),
    m_timeProfile(timeProfile)
{}

void Trip::setRoute(Route *r) {
    m_route = r;
}

void Trip::setStartTime(QTime t) {
    m_startTime = t;
}

void Trip::setTimeProfile(TimeProfile *p) {
    m_timeProfile = p;
}

Route *Trip::route()    { return m_route; }
QTime Trip::startTime() { return m_startTime; }

QTime Trip::endTime() {
    QTime d = duration();
    return m_startTime.addMSecs(d.msecsSinceStartOfDay());
}

QTime Trip::duration() {
    float durMinutes = m_timeProfile->duration();
    QTime t(0, 0, 0, 0);
    return t.addSecs(durMinutes * 60);
}

QTime Trip::busstopTime(Busstop *b) {
    TimeProfileItem *itm = m_timeProfile->busstop(b);
    if(!itm)
        return QTime(0, 0, 0, 0);
    
    return m_startTime.addSecs(itm->depValue() * 60);
}

QTime Trip::busstopTime(QString id) {
    TimeProfileItem *itm = m_timeProfile->busstop(id);
    if(!itm)
        return QTime(0, 0, 0, 0);
    
    return m_startTime.addSecs(itm->depValue() * 60);
}

WeekDays * Trip::weekDays()       { return m_weekDays; }
TimeProfile * Trip::timeProfile() { return m_timeProfile; }

bool Trip::goesPastMidnight() {
    return m_startTime > endTime();
}

bool Trip::busstopIsAfterMidnight(Busstop *b) {
    if(!route()->hasBusstop(b))
        return false;
    
    return m_startTime > busstopTime(b);
}

bool Trip::busstopIsAfterMidnight(QString id) {
    if(!route()->hasBusstop(id))
        return false;
    
    return m_startTime > busstopTime(id);
}

void Trip::overwrite(Trip &other) {
    setRoute(other.route());
    setStartTime(other.startTime());
    weekDays()->overwrite(*other.weekDays());
    setTimeProfile(other.timeProfile());
}

bool Trip::operator<(Trip &other) {
    return startTime() < other.startTime();
}
