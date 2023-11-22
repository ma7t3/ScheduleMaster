#include "ProjectData\trip.h"
#include "ProjectData\timeProfile.h"
#include "App/global.h"

Trip::Trip(QString id, Route *route, QTime startTime, TimeProfile *timeProfile, WeekDays weekDays) :
    AbstractProjectDataItem(id),
    m_route(route),
    m_startTime(startTime),
    m_weekDays(new WeekDays(weekDays)),
    m_timeProfile(timeProfile),
    m_repeat(false)
{}

void Trip::setRoute(Route *r) {
    m_route = r;
    refreshChilds();
}

void Trip::setStartTime(QTime t) {
    m_startTime = t;
    refreshChilds();
}

void Trip::setTimeProfile(TimeProfile *p) {
    m_timeProfile = p;
    refreshChilds();
}

void Trip::setRepeat(bool r) {
    bool before = m_repeat;
    m_repeat = r;
    if(!before) {
        setRepeatEnd(QTime(0, 0, 0, 0));
        setRepeatInterval(QTime(0, 10, 0, 0));
    }
    refreshChilds();
}

void Trip::setRepeatInterval(QTime t) {
    m_repeatInterval = t;
    refreshChilds();
}

void Trip::setRepeatEnd(QTime t) {
    m_repeatEnd = t;
    refreshChilds();
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

bool Trip::hasRepeat() { return m_repeat; }
QTime Trip::repeatInterval() { return m_repeatInterval; }
QTime Trip::repeatEnd() { return m_repeatEnd; }

// Endlosschleife, wenn es über 0 Uhr hinaus geht
int Trip::childCount() {
    if(!hasRepeat())
        return 0;
    
    QTime t = m_startTime;
    int repIntMSecs = this->repeatInterval().msecsSinceStartOfDay();
    for(int i = 0; true; i++) {
        t = t.addMSecs(repIntMSecs);
        if(t > m_repeatEnd || t < m_startTime)
            return i + 1;
    }
}

int Trip::repeatTimes() { return childCount(); }

void Trip::refreshChilds() {
    if(!hasRepeat()) {
        m_childTrips.clear();
        return;
    }

    int childCount = repeatTimes();
    for(int i = 0; i < childCount; i++) {
        int diff = (m_repeatInterval.msecsSinceStartOfDay() / 1000) * i;
        QTime newStartTime(m_startTime.addSecs(diff));
        QString newID = m_childIDs.count() > i ? m_childIDs[i] : global::getNewID();
        
        if(m_childTrips.count() > i) {
            Trip *t = m_childTrips[i];
            t->setRoute(m_route);
            t->setStartTime(newStartTime);
            t->setTimeProfile(m_timeProfile);
            t->setSelfChildId(newID);
            t->weekDays()->setCode(weekDays()->toCode());
        } else {
            Trip *t = new Trip(id(), m_route, newStartTime, m_timeProfile, *m_weekDays);
            t->setSelfChildId(newID);
            m_childTrips << t;
        }
    }
}

Trip *Trip::childAt(int i) {
    if(!hasRepeat())
        return this;
    
    if(i >= 0 && i < m_childTrips.count())
        return m_childTrips[i];

    return nullptr;
}

Trip * Trip::repetitionAt(int i) { return childAt(i); }

void Trip::setChildIdList(QStringList l) { m_childIDs = l; }

QStringList Trip::childIDs()
{
    QStringList list;
    for(int i = 0; i < m_childTrips.count(); i++)
        list << m_childTrips[i]->selfChildId();

    return list;
}

QString Trip::childIdAt(int i)
{
    if(i >= 0 && i < m_childTrips.count())
        return m_childTrips[i]->selfChildId();

    return "";
}

void Trip::setSelfChildId(QString id) { m_selfChildId = id; }

QString Trip::selfChildId()
{
    if(m_selfChildId == "")
        return id();
    
    return m_selfChildId;
}

void Trip::operator=(Trip &other) {
    setRoute(other.route());
    setStartTime(other.startTime());
    weekDays()->operator=(*other.weekDays());
    setTimeProfile(other.timeProfile());
}

bool Trip::operator<(Trip &other) {
    return startTime() < other.startTime();
}
