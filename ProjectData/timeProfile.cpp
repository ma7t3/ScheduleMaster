#include "ProjectData\timeProfile.h"

TimeProfileItem::TimeProfileItem(QString id) :
    _busstopId(id) {
    _seperateTimes = false;
}

TimeProfileItem::TimeProfileItem(Busstop *b) :
    _busstopId(b->id()) {
    _seperateTimes = false;
}


void TimeProfileItem::setArrValue(float v) {
    _arrValue = v;
    _seperateTimes = true;
}

void TimeProfileItem::setDepValue(float v) {
    _depValue = v;
}

void TimeProfileItem::removeArrValue() {
    _arrValue = 0;
    _seperateTimes = false;
}

void TimeProfileItem::setBusstopMode(int m) {
    if(m >= 0 && m <= 4)
        _busstopMode = m;
    else
        _busstopMode = busstopModeNormal;
}

QString TimeProfileItem::busstopId()  { return _busstopId; }
float TimeProfileItem::arrValue()     { return _seperateTimes ? _arrValue : _depValue; }
float TimeProfileItem::depValue()     { return _depValue; }
bool TimeProfileItem::hasSeperateTimes() { return _seperateTimes; }
int TimeProfileItem::busstopMode()    { return _busstopMode; }





TimeProfile::TimeProfile(QString id, QString name) :
    AbstractProjectDataItem(id),
    _name(name)
{}

void TimeProfile::setName(QString n)                            { _name = n; }
void TimeProfile::setDuration(float d)                          { _duration = d; }
void TimeProfile::addBusstop(TimeProfileItem * itm)             { _items << itm; }
void TimeProfile::setBusstopList(QList<TimeProfileItem *> list) { _items = list; }
void TimeProfile::addBusstopList(QList<TimeProfileItem *> list) {
    for(int i = 0; i < list.count(); i++)
        this->addBusstop(list[i]);
}

QString TimeProfile::name() { return _name; }
float TimeProfile::duration() { return _duration; }
QList<TimeProfileItem *> TimeProfile::busstops() { return _items; }

TimeProfileItem *TimeProfile::busstop(QString id) {
    for(int i = 0; i < _items.count(); i++)
        if(_items[i]->busstopId() == id)
            return _items[i];

    return nullptr;
}

TimeProfileItem *TimeProfile::busstop(Busstop *b) {
    if(!b)
        return nullptr;

    return busstop(b->id());
}


void TimeProfile::removeBusstop(QString) {
    for(int i = 0; i < _items.count(); i++)
        if(_items[i]->busstopId() == id())
            _items.remove(i);
}
