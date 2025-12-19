#include "TimeProfile.h"

#include "src/projectdata/Route.h"

TimeProfile::TimeProfile(QObject *parent, const QUuid &id, TimeProfile *original) :
    ProjectDataItem(parent, id, original) {}

TimeProfile::TimeProfile(QObject *parent, const QJsonObject &jsonObject) : ProjectDataItem(parent) {
    fromJson(jsonObject);
}

QString TimeProfile::name() const {
    return _data.name;
}

void TimeProfile::setName(const QString &name) {
    _data.name = name;
    emit changed();
}

float TimeProfile::duration() {
    float duration = 0;
    Route *r = findParent<Route>();

    for(RouteBusstopItem *b : r->busstops())
        duration = std::max(duration, item(b)->arrival());

    return duration;
}

PDISet<TimeProfileItem> TimeProfile::items() const {
    Route *r = findParent<Route>();
    PDISet<TimeProfileItem> items;
    const PDIList<RouteBusstopItem> busstops = r->busstops();

    for(RouteBusstopItem *b : busstops)
        items.add(item(b));

    return items;
}

TimeProfileItem *TimeProfile::item(RouteBusstopItem *busstop) const {
    for(TimeProfileItem *item : std::as_const(_data.items)) {
        if(item->routeBusstopItemID() == busstop->id())
            return item;
    }

    auto *self = const_cast<TimeProfile *>(this);
    TimeProfileItem *item = const_cast<TimeProfile *>(this)->createItem(busstop);
    _data.items.add(item);
    connect(item, &TimeProfileItem::changed, self, [self, item] { emit self->itemChanged(item); });
    return item;
}

QJsonObject TimeProfile::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();

    jsonObject.insert("name",  name());
    jsonObject.insert("items", items().toJson());

    return jsonObject;
}

TimeProfileItem *TimeProfile::createItem(RouteBusstopItem *item, QObject *parent) {
    return new TimeProfileItem(parent ? parent : this, item);
}

TimeProfileItem *TimeProfile::createItem(const QJsonObject &jsonObject) {
    return new TimeProfileItem(this, jsonObject);
}

void TimeProfile::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
    setName(jsonObject.value("name").toString(tr("Time Profile")));

    const QJsonArray items = jsonObject.value("items").toArray();
    for(const QJsonValue &val : items)
        _data.items.add(createItem(val.toObject()));
}
