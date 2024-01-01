#include "ProjectData\route.h"
#include "abstractprojectdataitem.h"

Route::Route(QString id, int code, QString name, LineDirection *direction) :
    AbstractProjectDataItem(id),
    _code(code),
    _direction(direction),
    _name(name)
{}

void Route::setCode(int c) { _code = c; }
void Route::setDirection(LineDirection *ld) { _direction = ld; }
void Route::setName(QString n) { _name = n; }

int Route::code() { return _code; }
LineDirection *Route::direction() { return _direction; }
QString Route::name() { return _name; }

void Route::addBusstop(Busstop * b) { _busstops << b; }

void Route::insertBusstop(int i , Busstop * b) {
    if(!b || i < 0 || i >= _busstops.count())
        return;
    
    _busstops.insert(i, b);
}

void Route::setBusstops(QList<Busstop *> list) { _busstops = list; }


int Route::busstopCount() { return _busstops.count(); }

Busstop * Route::busstopAt(int i) {
    if(i < 0 || i >= _busstops.count())
        return nullptr;
    
    return _busstops[i];
}

Busstop * Route::firstBusstop() {
    if(_busstops.count() == 0)
        return nullptr;
    
    return _busstops[0];
}

Busstop * Route::lastBusstop() {
    if(_busstops.count() == 0)
        return nullptr;
    
    return _busstops[_busstops.count() - 1];
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

QList<Busstop *> Route::busstops() { return _busstops; }


void Route::clearBusstopList() { _busstops.clear(); }


int Route::timeProfileCount() { return _timeProfiles.count(); }
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
    if(i + 1 > _timeProfiles.count())
        return nullptr;
    
    return _timeProfiles[i];
}

QList<TimeProfile *> Route::timeProfiles() { return _timeProfiles; }

void Route::addTimeProfile(TimeProfile *p) {
    _timeProfiles << p;
}

void Route::addTimeProfiles(QList<TimeProfile *> list) {
    for(int i = 0; i < list.count(); i++)
        _timeProfiles << list[i];
}

void Route::setTimeProfiles(QList<TimeProfile *> list) { _timeProfiles = list; }



void Route::removeTimeProfile(TimeProfile *p)
{
    if(!p)
        return;
    
    for(int i = 0; i < timeProfileCount(); i++)
        if(timeProfileAt(i) == p)
            _timeProfiles.removeAt(i);
}

void Route::removeTimeProfile(QString id)
{
    for(int i = 0; i < timeProfileCount(); i++)
        if(timeProfileAt(i)->id() == id)
            _timeProfiles.removeAt(i);
}

int Route::indexOfTimeProfile(TimeProfile* p)
{
    for(int i = 0; i < _timeProfiles.count(); i++)
        if(_timeProfiles[i] == p)
            return i;

    return -1;
}

void Route::overwrite(Route &other) {
    setCode(other.code());
    setDirection(other.direction());
    setName(other.name());
    setBusstops(other.busstops());
    setTimeProfiles(other.timeProfiles());
}

bool Route::operator<(Route &other) {
    return code() < other.code();
}
