#include "ProjectData\timeProfile.h"

TimeProfileItem::TimeProfileItem(QString id) :
    m_busstopId(id) {
    m_seperateTimes = false;
}

TimeProfileItem::TimeProfileItem(Busstop *b) :
    m_busstopId(b->id()) {
    m_seperateTimes = false;
}


void TimeProfileItem::setArrValue(float v) {
    m_arrValue = v;
    m_seperateTimes = true;
}

void TimeProfileItem::setDepValue(float v) {
    m_depValue = v;
}

void TimeProfileItem::removeArrValue() {
    m_arrValue = 0;
    m_seperateTimes = false;
}

void TimeProfileItem::setBusstopMode(int m) {
    if(m >= 0 && m <= 4)
        m_busstopMode = m;
    else
        m_busstopMode = busstopModeNormal;
}

QString TimeProfileItem::busstopId()  { return m_busstopId; }
float TimeProfileItem::arrValue()     { return m_seperateTimes ? m_arrValue : m_depValue; }
float TimeProfileItem::depValue()     { return m_depValue; }
bool TimeProfileItem::hasSeperateTimes() { return m_seperateTimes; }
int TimeProfileItem::busstopMode()    { return m_busstopMode; }





TimeProfile::TimeProfile(QString id, QString name) :
    AbstractProjectDataItem(id),
    m_name(name)
{}

void TimeProfile::setName(QString n)                            { m_name = n; }
void TimeProfile::setDuration(float d)                          { m_duration = d; }
void TimeProfile::addBusstop(TimeProfileItem * itm)             { m_items << itm; }
void TimeProfile::setBusstopList(QList<TimeProfileItem *> list) { m_items = list; }
void TimeProfile::addBusstopList(QList<TimeProfileItem *> list) {
    for(int i = 0; i < list.count(); i++)
        this->addBusstop(list[i]);
}

QString TimeProfile::name() { return m_name; }
float TimeProfile::duration() { return m_duration; }
QList<TimeProfileItem *> TimeProfile::busstops() { return m_items; }

TimeProfileItem *TimeProfile::busstop(QString id) {
    for(int i = 0; i < m_items.count(); i++)
        if(m_items[i]->busstopId() == id)
            return m_items[i];

    return nullptr;
}

TimeProfileItem *TimeProfile::busstop(Busstop *b) {
    if(!b)
        return nullptr;

    return busstop(b->id());
}


void TimeProfile::removeBusstop(QString) {
    for(int i = 0; i < m_items.count(); i++)
        if(m_items[i]->busstopId() == id())
            m_items.remove(i);
}
