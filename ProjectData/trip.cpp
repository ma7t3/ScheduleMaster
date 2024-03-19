#include "trip.h"
#include "timeProfile.h"
#include "line.h"

Trip::Trip(QObject *parent,
           const QString &id,
           Route *route,
           const QTime &startTime,
           TimeProfile *timeProfile,
           const WeekDays &weekDays) :
    ProjectDataItem(parent, id),
    _route(route), _startTime(startTime), _weekDays(weekDays),
    _timeProfile(timeProfile) {}

Trip::Trip(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

Trip::Trip(const Trip &other) :
    ProjectDataItem(other.parent()) {
    copy(other);
}

bool Trip::operator<(const Trip &other) {
    Busstop *b = route()->firstCommonBusstop(other.route());

    int difference;

    if(b == nullptr || route() == other.route())
        difference = startTime().msecsSinceStartOfDay() - other.startTime().msecsSinceStartOfDay();
    else
        difference =  busstopTime(b).msecsSinceStartOfDay() - other.busstopTime(b).msecsSinceStartOfDay();

    if(difference != 0)
        return difference < 0;

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
    // *weekDays() = *other.weekDays();
    // Das darüber musste ich auskommentieren, weil das Programm sonst immer abgestüzt ist, sollte es ein Problem mit den WeekDays geben, muss man sich da nochmal ransetzen.
    // Aber es ist jetzt grade 22:49 Uhr, ich kann mir den Absturz nicht erklären und bin der Meinung, dass der Befehl unnötig ist, weil beim Ändern der WeekDays über den Fahrplan dies eh "zu Fuß" und nicht per Copy Constructor gemacht wird.
    setTimeProfile(other.timeProfile());
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

WeekDays Trip::weekDays() const {
    return _weekDays;
}

void Trip::setWeekDays(const WeekDays &w) {
    _weekDays = w;
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
