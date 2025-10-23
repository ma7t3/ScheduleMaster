#include "RouteBusstopItem.h"

#include "ProjectData.h"

RouteBusstopItem::RouteBusstopItem(QObject *parent, Busstop *busstop, const QUuid &id, RouteBusstopItem *original) :
    ProjectDataItem(parent, id, original) {
    _data.busstop = busstop;
}

RouteBusstopItem::RouteBusstopItem(QObject *parent, const QUuid &id, RouteBusstopItem *original) :
    ProjectDataItem(parent, id, original) {
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

QJsonObject RouteBusstopItem::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("busstop", busstop()->idAsString());

    BusstopPlatform *defaultPlatform = this->defaultPlatform();
    jsonObject.insert("defaultPlatform",
                      defaultPlatform ? QJsonValue(defaultPlatform->idAsString())
                                      : QJsonValue::Null);

    return jsonObject;
}

void RouteBusstopItem::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);

    QUuid busstopID = QUuid::fromString(jsonObject.value("busstop").toString());
    QUuid defaultPlatformID = QUuid::fromString(jsonObject.value("defaultPlatform").toString());

    ProjectData *projectData = findParent<ProjectData>();
    if(!projectData)
        return;

    _data.busstop = projectData->busstop(busstopID);
    setDefaultPlatform(_data.busstop->platform(defaultPlatformID));
}
