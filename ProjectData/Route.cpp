#include "Route.h"

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

void Route::appendBusstop(RouteBusstopItem *busstop) {
    _data.busstops.append(busstop);
    emit changed();
}

void Route::insertBusstop(const int &index, RouteBusstopItem *busstop) {
    _data.busstops.insert(index, busstop);
    emit changed();
}

void Route::removeBusstop(RouteBusstopItem *busstop) {
    _data.busstops.remove(busstop);
    emit changed();
}

void Route::removeBusstop(const QUuid &id) {
    _data.busstops.remove(id);
    emit changed();
}

void Route::removeBusstopAt(const int &index) {
    _data.busstops.removeAt(index);
    emit changed();
}

QJsonObject Route::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("name", name());
    jsonObject.insert("code", code());
    LineDirection *ld = direction();
    jsonObject.insert("direction", ld ? ld->id().toString() : "");

    return jsonObject;
}

void Route::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
    setName(jsonObject.value("name").toString(tr("unnamed route")));
    setCode(jsonObject.value("code").toInt(0));
    setDirection(qobject_cast<Line *>(parent())->direction(QUuid::fromString(jsonObject.value("direction").toString())));
}

