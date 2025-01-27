#include "RouteBusstopItem.h"

RouteBusstopItem::RouteBusstopItem(QObject *parent, Busstop *busstop, const QUuid &id, const bool &isClone) :
    ProjectDataItem(parent, id, isClone) {
    _data.busstop = busstop;
}

RouteBusstopItem::RouteBusstopItem(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
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
