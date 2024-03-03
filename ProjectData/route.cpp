#include "line.h"
#include "route.h"
#include "ProjectDataItem.h"
#include "projectdata.h"

Route::Route(QObject *parent,
             const QString &id,
             const int &code,
             const QString &name,
             LineDirection *direction) :
    ProjectDataItem(parent, id),
    _code(code), _direction(direction), _name(name) {}

Route::Route(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

Route::Route(const Route &other) :
    ProjectDataItem(other.parent()) {
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
    ProjectDataItem::copy(other);
    setCode(other.code());
    setDirection(other.direction());
    setName(other.name());
    setBusstops(other.busstops());
    setTimeProfiles(other.timeProfiles());

    QList<TimeProfile *> newTimeProfiles;
    for(int i = 0; i < other.timeProfileCount(); i++) {
        TimeProfile *p = other.timeProfileAt(i);
        if(hasTimeProfile(p->id())) {
            TimeProfile *currentP = timeProfile(p->id());
            *currentP = *p;
            newTimeProfiles << currentP;
        } else {
            newTimeProfiles << p;
        }
    }
    setTimeProfiles(newTimeProfiles);
}

void Route::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);

    setCode(jsonObject.value("code").toInt(0));
    setName(jsonObject.value("name").toString(tr("unnamed route")));
    setDirection(dynamic_cast<Line *>(parent())->direction(jsonObject.value("direction").toString()));
    QJsonArray jBusstops, jTimeProfiles;
    jBusstops = jsonObject.value("busstops").toArray({});
    jTimeProfiles = jsonObject.value("timeProfiles").toArray({});

    for(int i = 0; i < jBusstops.count(); ++i)
        if(jBusstops[i].isString())
            //addBusstop(new Busstop(this, jBusstops[i].toObject()));
            addBusstop(static_cast<ProjectData *>(parent()->parent())->busstop(jBusstops[i].toString()));

    for(int i = 0; i < jTimeProfiles.count(); ++i)
        if(jTimeProfiles[i].isObject())
            addTimeProfile(new TimeProfile(this, jTimeProfiles[i].toObject()));
}

QJsonObject Route::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("code", code());
    jsonObject.insert("name", name());
    jsonObject.insert("direction", direction()->id());

    QJsonArray jBusstops, jTimeProfiles;
    for(int i = 0; i < busstopCount(); ++i)
        jBusstops.append(busstopAt(i)->id());

    for(int i = 0; i < timeProfileCount(); ++i)
        jTimeProfiles.append(timeProfileAt(i)->toJson());

    jsonObject.insert("busstops", jBusstops);
    jsonObject.insert("timeProfiles", jTimeProfiles);

    return jsonObject;
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
    if(!b)
        return;

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

bool Route::hasTimeProfile(const QString &id) const {
    for (int i = 0; i < timeProfileCount(); ++i)
        if(timeProfileAt(i)->id() == id)
            return true;

    return false;
}

void Route::setTimeProfiles(QList<TimeProfile *> list) {
    _timeProfiles = list;
}

void Route::addTimeProfile(TimeProfile *p) {
    if(!p)
        return;

    p->setParent(this);
    _timeProfiles << p;
}

void Route::addTimeProfiles(QList<TimeProfile *> list) {
    for(int i = 0; i < list.count(); i++)
        if(list[i]) {
            list[i]->setParent(this);
            _timeProfiles << list[i];
        }
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
