#include "line.h"
#include "route.h"
#include "ProjectDataItem.h"
#include "projectdata.h"

Route::Route(QObject *parent, const QString &id) :
    ProjectDataItem(parent, id),
    _code(1),
    _updateTimer(new QTimer(this)) {
    _updateTimer->setSingleShot(true);
    connect(_updateTimer, &QTimer::timeout, this, &Route::onUpdateTimerTimeout);
}

Route::Route(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent),
    _updateTimer(new QTimer(this)) {
    _updateTimer->setSingleShot(true);
    connect(_updateTimer, &QTimer::timeout, this, &Route::onUpdateTimerTimeout);
    fromJson(jsonObject);
}

Route::Route(const Route &other) :
    ProjectDataItem(other.parent()),
    _updateTimer(new QTimer(this)) {
    _updateTimer->setSingleShot(true);
    connect(_updateTimer, &QTimer::timeout, this, &Route::onUpdateTimerTimeout);
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

    emit changed(this);
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
            addBusstop(static_cast<ProjectData *>(parent()->parent())->busstop(jBusstops[i].toString()));

    for(int i = 0; i < jTimeProfiles.count(); ++i)
        if(jTimeProfiles[i].isObject())
            addTimeProfile(newTimeProfile(jTimeProfiles[i].toObject()));
}

void Route::onUpdateTimerTimeout() {
    if(!_addedTimeProfiles.isEmpty())
        emit timeProfilesAdded(_addedTimeProfiles);

    if(!_changedTimeProfiles.isEmpty())
        emit timeProfilesChanged(_changedTimeProfiles);

    if(!_removedTimeProfiles.isEmpty())
        emit timeProfilesRemoved(_removedTimeProfiles);

    if(!_addedBusstops.isEmpty())
        emit busstopsAdded(_addedBusstops);

    if(!_changedBusstops.isEmpty())
        emit busstopsChanged(_changedBusstops);

    if(!_removedBusstops.isEmpty()) {
        emit busstopsRemoved(_removedBusstops);
    }

    _addedTimeProfiles.clear();
    _changedTimeProfiles.clear();
    _removedTimeProfiles.clear();

    _addedBusstops.clear();
    _changedBusstops.clear();
    _removedBusstops.clear();
}

QJsonObject Route::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("code", code());
    jsonObject.insert("name", name());
    if(direction())
        jsonObject.insert("direction", direction()->id());
    else
        jsonObject.insert("direction", QJsonValue::Null);

    QJsonArray jBusstops, jTimeProfiles;
    for(int i = 0; i < busstopCount(); ++i)
        jBusstops.append(busstopAt(i)->id());

    for(int i = 0; i < timeProfileCount(); ++i)
        jTimeProfiles.append(timeProfileAt(i)->toJson());

    jsonObject.insert("busstops", jBusstops);
    jsonObject.insert("timeProfiles", jTimeProfiles);

    return jsonObject;
}

QList<TimeProfile *> Route::cloneTimeProfiles() const {
    QList<TimeProfile *> result;
    for(int i = 0; i < timeProfileCount(); i++) {
        TimeProfile *p = new TimeProfile(*timeProfileAt(i));
        p->setBusstops(p->cloneItems());
        result << p;
    }
    return result;
}

int Route::code() const {
    return _code;
}


void Route::setCode(const int &newCode) {
    _code = newCode;
    emit changed(this);
}

QString Route::name() const {
    return _name;
}

void Route::setName(const QString &newName) {
    _name = newName;
    emit changed(this);
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

Busstop *Route::firstCommonBusstop(Route *other) const {
    foreach(Busstop *b, _busstops) {
        if(other->hasBusstop(b))
            return b;
    }

    return nullptr;
}

int Route::indexOfBusstop(Busstop *b) const {
    for(int i = 0; i < busstopCount(); i++)
        if(busstopAt(i) == b)
            return i;

    return -1;
}

void Route::setBusstops(const QList<Busstop *> &newBusstops) {
    for(Busstop *b : _busstops)
        b->disconnect(this);

    _busstops = newBusstops;
    for(Busstop *b : _busstops)
        connect(b, &Busstop::changed, this, &Route::onBusstopChanged);
    emit changed(this);
}

void Route::addBusstop(Busstop * b) {
    if(!b)
        return;

    _busstops << b;
    connect(b, &Busstop::changed, this, &Route::onBusstopChanged);
    emit changed(this);
    onBusstopAdded(b);
}

void Route::insertBusstop(const int &index , Busstop * b) {
    if(!b || index < 0 || index > _busstops.count())
        return;
    
    _busstops.insert(index, b);
    connect(b, &Busstop::changed, this, &Route::onBusstopChanged);
    emit changed(this);
    onBusstopAdded(b);
}

void Route::removeBusstop(const int &index) {
    if(index < 0 || index >= _busstops.count())
        return;
    Busstop *b = _busstops[index];
    _busstops.remove(index);
    b->disconnect(this);
    emit changed(this);
    onBusstopRemoved(b);
}

void Route::clearBusstops() {
    _busstops.clear();
    emit changed(this);
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
    emit changed(this);
}

void Route::addTimeProfile(TimeProfile *p) {
    if(!p)
        return;

    _timeProfiles << p;
    emit changed(this);
    onTimeProfileAdded(p);
}

void Route::addTimeProfiles(QList<TimeProfile *> list) {
    for(int i = 0; i < list.count(); i++)
        if(list[i]) {
            _timeProfiles << list[i];
            onTimeProfileAdded(list[i]);
        }
    emit changed(this);
}

void Route::removeTimeProfile(TimeProfile *p) {
    if(!p)
        return;
    
    for(int i = 0; i < timeProfileCount(); i++)
        if(timeProfileAt(i) == p)
            _timeProfiles.removeAt(i);

    emit changed(this);
    onTimeProfileRemoved(p);
}

int Route::indexOfTimeProfile(TimeProfile* p) const {
    for(int i = 0; i < _timeProfiles.count(); i++)
        if(_timeProfiles[i] == p)
            return i;

    return -1;
}

TimeProfile *Route::newTimeProfile(QString id) {
    if(id.isEmpty())
        id = ProjectDataItem::getNewID();

    TimeProfile *p = new TimeProfile(this, id);
    connect(p, &TimeProfile::changed, this, &Route::onTimeProfileChanged);
    return p;
}

TimeProfile *Route::newTimeProfile(const QJsonObject &obj) {
    TimeProfile *p = new TimeProfile(this, obj);
    connect(p, &TimeProfile::changed, this, &Route::onTimeProfileChanged);
    return p;
}

TimeProfile *Route::newTimeProfile(const TimeProfile &newTimeProfile) {
    TimeProfile *p = new TimeProfile(newTimeProfile);
    p->setParent(this);
    connect(p, &TimeProfile::changed, this, &Route::onTimeProfileChanged);
    return p;
}

void Route::onBusstopAdded(Busstop *b) {
    if(_addedBusstops.indexOf(b) == -1)
        _addedBusstops << b;
    _updateTimer->start();
}

void Route::onBusstopChanged(Busstop *b) {
    if(_changedBusstops.indexOf(b) == -1)
        _changedBusstops << b;
    _updateTimer->start();
}

void Route::onBusstopRemoved(Busstop *b) {
    if(_removedBusstops.indexOf(b) == -1)
        _removedBusstops << b;
    _updateTimer->start();
}

void Route::onTimeProfileAdded(TimeProfile *p) {
    if(_addedTimeProfiles.indexOf(p) == -1)
        _addedTimeProfiles << p;
    _updateTimer->start(0);
}

void Route::onTimeProfileChanged(TimeProfile *p) {
    if(_changedTimeProfiles.indexOf(p) == -1)
        _changedTimeProfiles << p;
    _updateTimer->start(0);
}

void Route::onTimeProfileRemoved(TimeProfile *p) {
    if(_removedTimeProfiles.indexOf(p) == -1)
        _removedTimeProfiles << p;
    _updateTimer->start(0);
}
