#include "ProjectData\route.h"
#include "ProjectDataItem.h"

Route::Route(const QString &id, const int &code, const QString &name, LineDirection *direction) :
    ProjectDataItem(id),
    _code(code),
    _direction(direction),
    _name(name) {
}

Route::Route(const Route &other) {
    copy(other);
}


bool Route::operator<(const Route &other) {
    return code() < other.code();
}

Route Route::operator=(const Route &other) {
    copy(other);
    return *this;
}

void Route::copy(const Route &other) {
    setCode(other.code());
    setDirection(other.direction());
    setName(other.name());
    setBusstops(other.busstops());
    setTimeProfiles(other.timeProfiles());
}

int Route::code() const {
    return _code;
}


void Route::setCode(const int &newCode) {
    _code = newCode;
}

QString Route::name() const {
    return _name;
}

void Route::setName(const QString &newName) {
    _name = newName;
}

LineDirection *Route::direction() const {
    return _direction;
}

void Route::setDirection(LineDirection *newDirection) {
    _direction = newDirection;
}

QList<Busstop *> Route::busstops() const {
    return _busstops;
}

int Route::busstopCount() const {
    return _busstops.count();
}

Busstop * Route::busstopAt(const int &index) const {
    if(index < 0 || index >= _busstops.count())
        return nullptr;

    return _busstops[index];
}

Busstop * Route::firstBusstop() const {
    if(_busstops.count() == 0)
        return nullptr;

    return _busstops[0];
}

Busstop * Route::lastBusstop() const {
    if(_busstops.count() == 0)
        return nullptr;

    return _busstops[_busstops.count() - 1];
}

bool Route::hasBusstop(Busstop *b) const {
    for(int i = 0; i < this->busstopCount(); i++)
        if(this->busstopAt(i) == b)
            return true;

    return false;
}

bool Route::hasBusstop(const QString &id) const {
    for(int i = 0; i < this->busstopCount(); i++)
        if(this->busstopAt(i)->id() == id)
            return true;

    return false;
}

void Route::setBusstops(const QList<Busstop *> &newBusstops) {
    _busstops = newBusstops;
}

void Route::addBusstop(Busstop * b) {
    _busstops << b;
}

void Route::insertBusstop(const int &index , Busstop * b) {
    if(!b || index < 0 || index >= _busstops.count())
        return;
    
    _busstops.insert(index, b);
}

void Route::clearBusstops() {
    _busstops.clear();
}

QList<TimeProfile *> Route::timeProfiles() const {
    return _timeProfiles;
}

int Route::timeProfileCount() const {
    return _timeProfiles.count();
}

TimeProfile* Route::timeProfile(const QString &id) const {
    for(int i = 0; i < timeProfileCount(); i++)
        if(timeProfileAt(i)->id() == id)
            return timeProfileAt(i);

    return nullptr;
}

TimeProfile* Route::timeProfileAt(const int &i) const {
    if(i + 1 > _timeProfiles.count())
        return nullptr;

    return _timeProfiles[i];
}

TimeProfile *Route::timeProfileWithName(const QString &name) const {
    for(int i = 0; i < timeProfileCount(); i++)
        if(timeProfileAt(i)->name() == name)
            return timeProfileAt(i);

    return nullptr;
}

void Route::setTimeProfiles(QList<TimeProfile *> list) {
    _timeProfiles = list;
}

void Route::addTimeProfile(TimeProfile *p) {
    _timeProfiles << p;
}

void Route::addTimeProfiles(QList<TimeProfile *> list) {
    for(int i = 0; i < list.count(); i++)
        _timeProfiles << list[i];
}

void Route::removeTimeProfile(TimeProfile *p)
{
    if(!p)
        return;
    
    for(int i = 0; i < timeProfileCount(); i++)
        if(timeProfileAt(i) == p)
            _timeProfiles.removeAt(i);
}

void Route::removeTimeProfile(const QString &id) {
    for(int i = 0; i < timeProfileCount(); i++)
        if(timeProfileAt(i)->id() == id)
            _timeProfiles.removeAt(i);
}

int Route::indexOfTimeProfile(TimeProfile* p) const {
    for(int i = 0; i < _timeProfiles.count(); i++)
        if(_timeProfiles[i] == p)
            return i;

    return -1;
}
