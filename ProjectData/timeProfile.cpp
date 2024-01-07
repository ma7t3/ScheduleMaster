#include "ProjectData\timeProfile.h"

TimeProfileItem::TimeProfileItem(const QString &busstopId) :
    _busstopId(busstopId) {
    _seperateTimes = false;
}

TimeProfileItem::TimeProfileItem(Busstop *b) :
    _busstopId(b->id()) {
    _seperateTimes = false;
}

TimeProfileItem::TimeProfileItem(const TimeProfileItem &other) {
    copy(other);
}

TimeProfileItem TimeProfileItem::operator=(const TimeProfileItem &other) {
    copy(other);
    return *this;
}


void TimeProfileItem::copy(const TimeProfileItem &other) {
    if(other.hasSeperateTimes())
        setArrValue(other.arrValue());

    setDepValue(other.depValue());
    setArrValue(other.busstopMode());
}

float TimeProfileItem::arrValue() const {
    return _seperateTimes ? _arrValue : _depValue;
}

void TimeProfileItem::setArrValue(const float &newArrValue) {
    _arrValue = newArrValue;
    _seperateTimes = true;
}

void TimeProfileItem::removeArrValue() {
    _arrValue = 0;
    _seperateTimes = false;
}

float TimeProfileItem::depValue() const {
    return _depValue;
}
void TimeProfileItem::setDepValue(const float &newDepValue) {
    _depValue = newDepValue;
}

int TimeProfileItem::busstopMode() const {
    return _busstopMode;
}

void TimeProfileItem::setBusstopMode(const int &newBusstopMode) {
    if(newBusstopMode >= 0 && newBusstopMode <= 4)
        _busstopMode = newBusstopMode;
    else
        _busstopMode = BusstopModeNormal;
}

QString TimeProfileItem::busstopId() const {
    return _busstopId;
}

bool TimeProfileItem::hasSeperateTimes() const {
    return _seperateTimes;
}

TimeProfile::TimeProfile(const QString &id, const QString &name) :
    ProjectDataItem(id), _name(name) {
}

TimeProfile::TimeProfile(const TimeProfile &other) {
    copy(other);
}

TimeProfile TimeProfile::operator=(const TimeProfile &other) {
    copy(other);
    return *this;
}

void TimeProfile::copy(const TimeProfile &other) {
    setName(other.name());
    setDuration(other.duration());
}

QString TimeProfile::name() const {
    return _name;
}

void TimeProfile::setName(const QString &newName) {
    _name = newName;
}

float TimeProfile::duration() const {
    return _duration;
}

void TimeProfile::setDuration(const float &newDuration) {
    _duration = newDuration;
}

QList<TimeProfileItem *> TimeProfile::busstops() const {
    return _items;
}

TimeProfileItem *TimeProfile::busstop(Busstop *b) const {
    if(!b)
        return nullptr;

    return busstop(b->id());
}

TimeProfileItem *TimeProfile::busstop(const QString &id) const {
    for(int i = 0; i < _items.count(); i++)
        if(_items[i]->busstopId() == id)
            return _items[i];

    return nullptr;
}

void TimeProfile::setBusstops(const QList<TimeProfileItem *> &newBusstops) {
    _items = newBusstops;
}

void TimeProfile::addBusstop(TimeProfileItem * itm) {
    _items << itm;
}

void TimeProfile::addBusstops(const QList<TimeProfileItem *> &busstops) {
    for(int i = 0; i < busstops.count(); i++)
        this->addBusstop(busstops[i]);
}

void TimeProfile::removeBusstop(const QString &id) {
    for(int i = 0; i < _items.count(); i++)
        if(_items[i]->busstopId() == id)
            _items.remove(i);
}

