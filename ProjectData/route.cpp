#include "ProjectData\route.h"
#include "abstractprojectdataitem.h"

Route::Route(QString id, int code, QString name, LineDirection *direction) :
    AbstractProjectDataItem(id),
    m_code(code),
    m_direction(direction),
    m_name(name)
{}

void Route::setCode(int c) { m_code = c; }
void Route::setDirection(LineDirection *ld) { m_direction = ld; }
void Route::setName(QString n) { m_name = n; }

int Route::code() { return m_code; }
LineDirection *Route::direction() { return m_direction; }
QString Route::name() { return m_name; }

void Route::addBusstop(Busstop * b) { m_busstops << b; }

void Route::insertBusstop(int i , Busstop * b) {
    if(!b || i < 0 || i >= m_busstops.count())
        return;
    
    m_busstops.insert(i, b);
}

void Route::setBusstopList(QList<Busstop *> list) { m_busstops = list; }


int Route::busstopCount() { return m_busstops.count(); }

Busstop * Route::busstopAt(int i) {
    if(i < 0 || i >= m_busstops.count())
        return nullptr;
    
    return m_busstops[i];
}

Busstop * Route::firstBusstop() {
    if(m_busstops.count() == 0)
        return nullptr;
    
    return m_busstops[0];
}

Busstop * Route::lastBusstop() {
    if(m_busstops.count() == 0)
        return nullptr;
    
    return m_busstops[m_busstops.count() - 1];
}

bool Route::hasBusstop(Busstop *b) {
    for(int i = 0; i < this->busstopCount(); i++)
        if(this->busstopAt(i) == b)
            return true;

    return false;
}

bool Route::hasBusstop(QString id) {
    for(int i = 0; i < this->busstopCount(); i++)
        if(this->busstopAt(i)->id() == id)
            return true;

    return false;
}

QList<Busstop *> Route::busstops() { return m_busstops; }


void Route::clearBusstopList() { m_busstops.clear(); }


int Route::timeProfileCount() { return m_timeProfiles.count(); }
int Route::profileCount() { return timeProfileCount(); }

TimeProfile* Route::timeProfile(QString id) {
    for(int i = 0; i < timeProfileCount(); i++)
        if(timeProfileAt(i)->id() == id)
            return timeProfileAt(i);

    return nullptr;
}

TimeProfile *Route::timeProfileWithName(QString name) {
    for(int i = 0; i < timeProfileCount(); i++)
        if(timeProfileAt(i)->name() == name)
            return timeProfileAt(i);

    return nullptr;
}

TimeProfile* Route::timeProfileAt(int i) {
    if(i + 1 > m_timeProfiles.count())
        return nullptr;
    
    return m_timeProfiles[i];
}

QList<TimeProfile *> Route::timeProfiles() { return m_timeProfiles; }

void Route::addTimeProfile(TimeProfile *p) {
    m_timeProfiles << p;
}

void Route::addTimeProfileList(QList<TimeProfile *> list) {
    for(int i = 0; i < list.count(); i++)
        m_timeProfiles << list[i];
}

void Route::setTimeProfileList(QList<TimeProfile *> list) { m_timeProfiles = list; }



void Route::removeTimeProfile(TimeProfile *p)
{
    if(!p)
        return;
    
    for(int i = 0; i < timeProfileCount(); i++)
        if(timeProfileAt(i) == p)
            m_timeProfiles.removeAt(i);
}

void Route::removeTimeProfile(QString id)
{
    for(int i = 0; i < timeProfileCount(); i++)
        if(timeProfileAt(i)->id() == id)
            m_timeProfiles.removeAt(i);
}

int Route::indexOfTimeProfile(TimeProfile* p)
{
    for(int i = 0; i < m_timeProfiles.count(); i++)
        if(m_timeProfiles[i] == p)
            return i;

    return -1;
}
