#include "RouteBusstopItem.h"

RouteBusstopItem::RouteBusstopItem(QObject *parent, Busstop *busstop, const QUuid &id) :
    ProjectDataItem(parent, id) {
    _data.busstop = busstop;
}

RouteBusstopItem::RouteBusstopItem(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

RouteBusstopItem::Data RouteBusstopItem::data() const {
    return _data;
}

void RouteBusstopItem::setData(const Data &newData) {
    _data = newData;
    emit changed();
}

Busstop *RouteBusstopItem::busstop() const {
    return _data.busstop;
}

BusstopPlatform *RouteBusstopItem::defaultPlatform() const {
    return _data.defaultPlatform;
}

void RouteBusstopItem::setDefaultPlatform(BusstopPlatform *platform) {
    _data.defaultPlatform = platform;
    emit changed();
}

QJsonObject RouteBusstopItem::toJson() const {}

void RouteBusstopItem::fromJson(const QJsonObject &jsonObject) {}
