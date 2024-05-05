#include "ProjectData\timeProfile.h"
#include "projectdata.h"

TimeProfileItem::TimeProfileItem(QObject *parent, const QString &busstopId) :
    ProjectDataItem(parent),
    _arrValue(0),
    _depValue(0),
    _seperateTimes(false),
    _busstopMode(1),
    _busstopId(busstopId) {
}

TimeProfileItem::TimeProfileItem(QObject *parent, Busstop *b) :
    ProjectDataItem(parent),
    _arrValue(0),
    _depValue(0),
    _seperateTimes(false),
    _busstopMode(1),
    _busstopId(b->id()) {
}

TimeProfileItem::TimeProfileItem(QObject *parent, const QJsonObject  &jsonObject) :
    ProjectDataItem(parent),
    _seperateTimes(false) {
    fromJson(jsonObject);
}

TimeProfileItem::TimeProfileItem(const TimeProfileItem &other) :
    ProjectDataItem(other.parent()) {
    copy(other);
}

TimeProfileItem TimeProfileItem::operator=(const TimeProfileItem &other) {
    copy(other);
    return *this;
}

void TimeProfileItem::copy(const TimeProfileItem &other) {
    ProjectDataItem::copy(other);
    _busstopId = other.busstopId();

    if(other.hasSeperateTimes())
        setArrValue(other.arrValue());

    setDepValue(other.depValue());
    setBusstopMode(other.busstopMode());
}

void TimeProfileItem::fromJson(const QJsonObject &jsonObject) {
    _busstopId = jsonObject.value("busstopID").toString();

    if(jsonObject.contains("arr"))
        setArrValue(jsonObject.value("arr").toDouble(0));

    setDepValue(jsonObject.value("dep").toDouble(0));
    setBusstopMode(jsonObject.value("busstopMode").toInt(1));
}

QJsonObject TimeProfileItem::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.remove("id");
    jsonObject.insert("busstopID", busstopId());
    jsonObject.insert("busstopMode", busstopMode());


    //qDebug() << dynamic_cast<ProjectData *>(parent()->parent()->parent()->parent())->busstop(busstopId())->name() << ": " << hasSeperateTimes() << " - " << arrValue();
    //qDebug() << hasSeperateTimes() << " - " << arrValue();

    if(hasSeperateTimes()) {
        qDebug() << arrValue();
        jsonObject.insert("arr", arrValue());
    }

    jsonObject.insert("dep", depValue());

    return jsonObject;
}

float TimeProfileItem::arrValue() const {
    return _seperateTimes ? _arrValue : _depValue;
}

void TimeProfileItem::setArrValue(const float &newArrValue) {
    _arrValue = newArrValue;
    _seperateTimes = true;
}

void TimeProfileItem::removeArrValue() {
    _arrValue = 0;
    _seperateTimes = false;
}

float TimeProfileItem::depValue() const {
    return _depValue;
}
void TimeProfileItem::setDepValue(const float &newDepValue) {
    _depValue = newDepValue;
}

int TimeProfileItem::busstopMode() const {
    return _busstopMode;
}

void TimeProfileItem::setBusstopMode(const int &newBusstopMode) {
    if(newBusstopMode >= 0 && newBusstopMode <= 4)
        _busstopMode = newBusstopMode;
    else
        _busstopMode = BusstopModeNormal;
}

QString TimeProfileItem::busstopId() const {
    return _busstopId;
}

bool TimeProfileItem::hasSeperateTimes() const {
    return _seperateTimes;
}

TimeProfile::TimeProfile(QObject *parent, const QString &id) :
    ProjectDataItem(parent, id) {

}

TimeProfile::TimeProfile(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

TimeProfile::TimeProfile(const TimeProfile &other) :
    ProjectDataItem(other.parent()) {
    copy(other);
}

TimeProfile TimeProfile::operator=(const TimeProfile &other) {
    copy(other);
    return *this;
}

void TimeProfile::copy(const TimeProfile &other) {
    ProjectDataItem::copy(other);
    setName(other.name());
    setDuration(other.duration());
    setBusstops(other.busstops());
}


void TimeProfile::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
    setDuration(jsonObject.value("duration").toDouble(0));
    setName(jsonObject.value("name").toString(tr("unnamed time profile")));

    QJsonArray jItems = jsonObject.value("items").toArray();
    for(int i = 0; i < jItems.count(); ++i)
        if(jItems[i].isObject())
            addBusstop(new TimeProfileItem(this, jItems[i].toObject()));
}

QJsonObject TimeProfile::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("name", name());
    jsonObject.insert("duration", duration());
    QList<TimeProfileItem *> items = busstops();

    QJsonArray jItems;
    for (int i = 0; i < items.count(); ++i)
        jItems.append(items[i]->toJson());

    jsonObject.insert("items", jItems);

    return jsonObject;
}

void TimeProfile::refreshChilds() {
    foreach (TimeProfileItem *itm, _items)
        itm->setParent(this);
}

QString TimeProfile::name() const {
    return _name;
}

void TimeProfile::setName(const QString &newName) {
    _name = newName;
}

float TimeProfile::duration() const {
    return _duration;
}

void TimeProfile::setDuration(const float &newDuration) {
    _duration = newDuration;
}

QList<TimeProfileItem *> TimeProfile::busstops() const {
    return _items;
}

TimeProfileItem *TimeProfile::busstop(Busstop *b) const {
    if(!b)
        return nullptr;

    return busstop(b->id());
}

TimeProfileItem *TimeProfile::busstop(const QString &id) const {
    for(int i = 0; i < _items.count(); i++)
        if(_items[i]->busstopId() == id)
            return _items[i];

    return nullptr;
}

void TimeProfile::setBusstops(const QList<TimeProfileItem *> &newBusstops) {
    _items = newBusstops;
}

void TimeProfile::addBusstop(TimeProfileItem * itm) {
    if(!itm)
        return;

    itm->setParent(this);
    _items << itm;
}

void TimeProfile::addBusstops(const QList<TimeProfileItem *> &busstops) {
    for(int i = 0; i < busstops.count(); i++)
        if(busstops[i]) {
            busstops[i]->setParent(this);
            this->addBusstop(busstops[i]);
        }
}

void TimeProfile::removeBusstop(const QString &id) {
    for(int i = 0; i < _items.count(); i++)
        if(_items[i]->busstopId() == id)
            _items.remove(i);
}

