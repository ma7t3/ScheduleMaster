#include "trip.h"
#include "timeProfile.h"
#include "line.h"

Trip::Trip(QObject *parent, const QString &id) :
    ProjectDataItem(parent, id) {}

Trip::Trip(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

Trip::Trip(const Trip &other) :
    ProjectDataItem(other.parent()) {
    copy(other);
}

bool Trip::operator<(const Trip &other) {
    if(startTime() !=  other.startTime())
        return startTime() < other.startTime();

    if(route() != other.route())
        return route() < other.route();

    if(_weekDays != other.weekDays())
        return _weekDays < other.weekDays();

    return false;
}

Trip Trip::operator=(const Trip &other) {
    copy(other);
    return *this;
}

void Trip::copy(const Trip &other) {
    ProjectDataItem::copy(other);
    setRoute(other.route());
    setStartTime(other.startTime());
    setWeekDays(other.weekDays());
    setTimeProfile(other.timeProfile());

    emit changed(this);
}

void Trip::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);

    setStartTime(QTime::fromMSecsSinceStartOfDay(jsonObject.value("startTime").toInt()));
    setRoute(dynamic_cast<Line *>(parent())->route(jsonObject.value("routeID").toString()));
    setTimeProfile(route()->timeProfile(jsonObject.value("timeProfileID").toString()));
    _weekDays.setCode(jsonObject.value("weekdays").toInt());
}


QJsonObject Trip::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();

    jsonObject.insert("startTime", startTime().msecsSinceStartOfDay());
    jsonObject.insert("weekdays", _weekDays.toCode());
    jsonObject.insert("routeID", route()->id());
    jsonObject.insert("timeProfileID", timeProfile()->id());

    return jsonObject;
}

Route *Trip::route() const {
    return _route;
}

void Trip::setRoute(Route *r) {
    _route = r;
    emit changed(this);
}

QTime Trip::startTime() const {
    return _startTime;
}

void Trip::setStartTime(const QTime &newStartTime) {
    _startTime = newStartTime;
    emit changed(this);
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
        return QTime();
    
    return _startTime.addSecs(itm->depValue() * 60);
}

QTime Trip::busstopTime(const QString &id) const {
    TimeProfileItem *itm = _timeProfile->busstop(id);
    if(!itm)
        return QTime();
    
    return _startTime.addSecs(itm->depValue() * 60);
}

TimeProfile * Trip::timeProfile() const {
    return _timeProfile;
}

void Trip::setTimeProfile(TimeProfile *p) {
    _timeProfile = p;
    emit changed(this);
}

WeekDays Trip::weekDays() const {
    return _weekDays;
}

void Trip::setWeekDays(const WeekDays &w) {
    _weekDays = w;
    emit changed(this);
    qDebug() << "weekdays updated";
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
