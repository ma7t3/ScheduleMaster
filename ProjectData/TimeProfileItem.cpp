#include "TimeProfileItem.h"

#include "ProjectData/ProjectData.h"

TimeProfileItem::TimeProfileItem(QObject *parent, RouteBusstopItem *busstop, const QUuid &id,
                                 const bool &isClone) : ProjectDataItem(parent, id, isClone) {
    _data.busstopID = busstop->id();
}

TimeProfileItem::TimeProfileItem(QObject *parent, const QUuid &id, const bool &isClone) :
    ProjectDataItem(parent, id, isClone) {}

TimeProfileItem::TimeProfileItem(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

QUuid TimeProfileItem::routeBusstopItemID() const {
    return _data.busstopID;
}

bool TimeProfileItem::hasArrival() const {
    return _data.arrival != -1;
}

float TimeProfileItem::arrival() const {
    return hasArrival() ? _data.arrival : _data.departure;
}

void TimeProfileItem::setArrival(const float &value) {
    _data.arrival = value;
    emit changed();
}

void TimeProfileItem::removeArrival() {
    _data.arrival = -1;
    emit changed();
}

float TimeProfileItem::departure() const {
    return _data.departure;
}

void TimeProfileItem::setDeparture(const float &value) {
    _data.departure = value;
    emit changed();
}

QJsonObject TimeProfileItem::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();

    jsonObject.insert("busstopID", _data.busstopID.toString(QUuid::WithoutBraces));

    jsonObject.insert("dep", departure());
    jsonObject.insert("arr", hasArrival() ? QJsonValue(arrival()) : QJsonValue::Null);

    return jsonObject;
}

void TimeProfileItem::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
    // TODO

    const QUuid busstopID = QUuid::fromString(jsonObject.value("busstopID").toString());

    Route *r = findParent<Route>();
    RouteBusstopItem *item = r->busstop(busstopID);

    if(item) {
        _data.busstopID = busstopID;
    } else { // SUPPORT FOR OLDER VERSIONS
        ProjectData *pd = findParent<ProjectData>();
        Busstop *b = pd->busstop(busstopID);
        for(RouteBusstopItem *item : r->busstops())
            if(item->busstop() == b)
                _data.busstopID = item->id();
    }

    setDeparture(jsonObject.value("dep").toDouble(0));
    setArrival(jsonObject.value("arr").toDouble(-1));
}
