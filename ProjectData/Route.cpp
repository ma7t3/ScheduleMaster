#include "Route.h"

#include "ProjectData.h"
#include "Line.h"

Route::Route(QObject *parent, const QUuid &id, const bool &isClone) :
    ProjectDataItem(parent, id, isClone) {
}

Route::Route(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

bool Route::operator<(const Route &other) const {
    return code() < other.code();
}

QString Route::name() const {
    return _data.name;
}

void Route::setName(const QString &newName) {
    _data.name = newName;
    emit changed();
}

int Route::code() const {
    return _data.code;
}

void Route::setCode(const int &newCode) {
    _data.code = newCode;
    emit changed();
}

LineDirection *Route::direction() const {
    LineDirection *ld = _data.direction;
    if(!ld)
        return nullptr;

    return ld->inUse() ? ld : nullptr;
}

void Route::setDirection(LineDirection *newDirection) {
    if(newDirection && newDirection->inUse())
        _data.direction = newDirection;
    emit changed();
}

RouteBusstopItem *Route::createItem(Busstop *b, QObject *parent) {
    return new RouteBusstopItem(parent ? parent : this, b);
}

RouteBusstopItem *Route::createItem(const QJsonObject &jsonObject) {
    return new RouteBusstopItem(this, jsonObject);
}

TimeProfile *Route::createTimeProfile(QObject *parent) {
    return new TimeProfile(parent ? parent : this);
}

TimeProfile *Route::createTimeProfile(const QJsonObject &jsonObject) {
    return new TimeProfile(this, jsonObject);
}

int Route::busstopCount() const {
    return _data.busstops.count();
}

RouteBusstopItem *Route::busstop(const QUuid &id) const {
    return _data.busstops.find(id);
}

PDIList<RouteBusstopItem> Route::busstops() const {
    return _data.busstops;
}

bool Route::containsBusstop(Busstop *busstop) const {
    return _data.busstops.filterOne([busstop](RouteBusstopItem *item){return item->busstop() == busstop;});
}

bool Route::containsBusstop(const QUuid &id) const {
    return _data.busstops.filterOne([id](RouteBusstopItem *item){return item->id() == id;});
}

PDIList<RouteBusstopItem> Route::itemsWithBusstop(Busstop *busstop) const {
    return _data.busstops.filter([busstop](RouteBusstopItem *item){return item->busstop() == busstop;});
}

PDIList<RouteBusstopItem> Route::itemsWithBusstop(const QUuid &id) const {
    return _data.busstops.filter([id](RouteBusstopItem *item){return item->id() == id;});
}

RouteBusstopItem *Route::firstBusstop() const {
    if(!_data.busstops.isEmpty())
        return _data.busstops.first();
    else
        return nullptr;
}

RouteBusstopItem *Route::lastBusstop() const {
    if(!_data.busstops.isEmpty())
        return _data.busstops.last();
    else
        return nullptr;
}

PDIList<RouteBusstopItem> Route::commonBusstops(Route *route, const bool &sameDefaultPlatform) const {
    PDIList<RouteBusstopItem> busstops = _data.busstops.filter([route](RouteBusstopItem *item){return route->containsBusstop(item->busstop());});

    if(!sameDefaultPlatform)
        return busstops;

    return busstops.filter([route](RouteBusstopItem *item){
        BusstopPlatform *platform = item->defaultPlatform();

        return route->busstops().some([platform](RouteBusstopItem *item){
            return item->defaultPlatform() == platform;
        });
    });
}

Busstop *Route::firstCommonBusstop(Route *route, const bool &sameDefaultPlatform) const {
    PDIList<RouteBusstopItem> busstops = commonBusstops(route, sameDefaultPlatform);
    if(busstops.isEmpty())
        return nullptr;

    return busstops.first()->busstop();
}

Busstop *Route::lastCommonBusstop(Route *route, const bool &sameDefaultPlatform) const {
    PDIList<RouteBusstopItem> busstops = commonBusstops(route, sameDefaultPlatform);
    if(busstops.isEmpty())
        return nullptr;

    return busstops.last()->busstop();
}

int Route::indexOfBusstop(RouteBusstopItem *busstopItem) const {
    return _data.busstops.indexOf(busstopItem);
}

void Route::appendBusstop(RouteBusstopItem *busstop) {
    insertBusstop(busstopCount(), busstop);
}

void Route::insertBusstop(const int &index, RouteBusstopItem *busstop) {
    _data.busstops.insert(index, busstop);
    connect(busstop, &Busstop::changed, this, [this, busstop]() {
        emit busstopChanged(indexOfBusstop(busstop), busstop);
    });
    emit busstopAdded(index, busstop);
    emit changed();
}

void Route::moveBusstop(const int &from, const int &to) {
    if(from < 0 || from >= busstopCount() || to < 0 || to >= busstopCount())
        return;

    _data.busstops.insert(to, _data.busstops.takeAt(from));
    emit busstopMoved(from, to);
}

void Route::moveBusstop(RouteBusstopItem *busstop, const int &to) {
    moveBusstop(indexOfBusstop(busstop), to);
}

void Route::removeBusstop(RouteBusstopItem *busstop) {
    const int index = indexOfBusstop(busstop);
    _data.busstops.remove(busstop);
    busstop->disconnect(this);
    emit busstopRemoved(index, busstop);
    emit changed();
}

void Route::removeBusstop(const QUuid &id) {
    removeBusstop(busstop(id));
}

void Route::removeBusstopAt(const int &index) {
    if(index < 0 || index >= busstopCount())
        return;

    RouteBusstopItem *b = _data.busstops.at(index);
    removeBusstop(b);
}

void Route::reverseBusstopOrder() {
    int n = _data.busstops.count() - 1;
    for (int i = 0; i < n; i++) {
        _data.busstops.insert(i, _data.busstops.takeAt(n));
        emit busstopMoved(n, i);
    }
}

int Route::timeProfileCount() const {
    return _data.timeProfiles.count();
}

TimeProfile *Route::timeProfile(const QUuid &id) const {
    return _data.timeProfiles.find(id);
}

PDIList<TimeProfile> Route::timeProfiles() const {
    return _data.timeProfiles;
}

int Route::indexOfTimeProfile(TimeProfile *timeProfile) {
    return _data.timeProfiles.indexOf(timeProfile);
}

void Route::appendTimeProfile(TimeProfile *timeProfile) {
    insertTimeProfile(timeProfileCount(), timeProfile);
}

void Route::insertTimeProfile(const int &index, TimeProfile *timeProfile) {
    _data.timeProfiles.insert(index, timeProfile);
    connect(timeProfile, &TimeProfile::itemChanged, this, [this, timeProfile]{
        emit timeProfileChanged(indexOfTimeProfile(timeProfile), timeProfile);
    });
    emit timeProfileAdded(index, timeProfile);
    emit changed();
}

void Route::moveTimeProfile(const int &from, const int &to) {
    if(from < 0 || from >= timeProfileCount() || to < 0 || to >= timeProfileCount())
        return;

    _data.busstops.insert(to, _data.busstops.takeAt(from));
    emit timeProfileMoved(from, to);
}

void Route::moveTimeProfile(TimeProfile *timeProfile, const int &to) {
    moveTimeProfile(indexOfTimeProfile(timeProfile), to);
}

void Route::removeTimeProfile(TimeProfile *timeProfile) {
    const int index = indexOfTimeProfile(timeProfile);
    _data.timeProfiles.remove(timeProfile);
    timeProfile->disconnect(this);
    emit timeProfileRemoved(index, timeProfile);
    emit changed();
}

void Route::removeTimeProfile(const QUuid &id) {
    removeTimeProfile(timeProfile(id));
}

void Route::removeTimeProfile(const int &index) {
    if(index < 0 || index >= timeProfileCount())
        return;

    TimeProfile *p = _data.timeProfiles.at(index);
    removeTimeProfile(p);
}

QJsonObject Route::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();

    jsonObject.insert("name", name());
    jsonObject.insert("code", code());
    LineDirection *ld = direction();
    jsonObject.insert("direction", ld ? QJsonValue(ld->id().toString()) : QJsonValue::Null);

    jsonObject.insert("busstops",     busstops().toJson());
    jsonObject.insert("timeProfiles", timeProfiles().toJson());

    return jsonObject;
}

void Route::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
    setName(jsonObject.value("name").toString(tr("unnamed route")));
    setCode(jsonObject.value("code").toInt(0));
    setDirection(qobject_cast<Line *>(parent())->direction(QUuid::fromString(jsonObject.value("direction").toString())));

    const QJsonArray busstopsArray = jsonObject.value("busstops").toArray();
    for(const QJsonValue &val : busstopsArray) {
        if(val.isString()) { // Compatibility for older file versions
            const QUuid id = QUuid::fromString(val.toString());
            Busstop *b = findParent<ProjectData>()->busstop(id);
            appendBusstop(createItem(b));
        } else {
            appendBusstop(createItem(val.toObject()));
        }
    }

    const QJsonArray profilesArray = jsonObject.value("timeProfiles").toArray();
    for(const QJsonValue &val : profilesArray) {
        appendTimeProfile(createTimeProfile(val.toObject()));
    }
}

