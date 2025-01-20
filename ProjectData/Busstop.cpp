#include "Busstop.h"

#include <QJsonArray>

Busstop::Busstop(QObject *parent, const QUuid &id) :
    ProjectDataItem(parent, id) {
}

Busstop::Busstop(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

bool Busstop::operator<(const Busstop &other) const {
    return name() < other.name();
}

Busstop::Data Busstop::data() const {
    return _data;
}

void Busstop::setData(const Data &newData) {
    _data = newData;
    emit changed();
}

QString Busstop::name() const {
    return _data.name;
}

void Busstop::setName(const QString &newName) {
    _data.name = newName;
    emit changed();
}

BusstopFlags Busstop::flags() const {
    return _data.flags;
}

void Busstop::setFlags(const BusstopFlags &newFlags) {
    _data.flags = newFlags;
    emit changed();
}

int Busstop::platformCount() const {
    return _data.platforms.count();
}

BusstopPlatform *Busstop::platform(const QUuid &id) const {
    return _data.platforms.contains(id) ? _data.platforms[id] : nullptr;
}

QList<BusstopPlatform *> Busstop::platforms() const {
    return _data.platforms.values();
}

void Busstop::addPlatform(BusstopPlatform *platform) {
    if(!platform)
        return;

    _data.platforms.insert(platform->id(), platform);
}

void Busstop::removePlatform(BusstopPlatform *platform) {
    if(!platform)
        return;

    removePlatform(platform->id());
}

void Busstop::removePlatform(const QUuid &id) {
    _data.platforms.remove(id);
}

QList<BusstopPlatform *> Busstop::platformsWithFlag(const BusstopPlatformFlag &flag) {
    QList<BusstopPlatform *> result;
    for(BusstopPlatform *platform : _data.platforms)
        if(platform->flags().testFlag(flag))
            result << platform;

    return result;
}

BusstopPlatform *Busstop::defaultPlatform() const {
    if(!_data.defaultPlatform)
        return nullptr;

    return _data.platforms.contains(_data.defaultPlatform->id()) ? _data.defaultPlatform : nullptr;
}

bool Busstop::isDefaultPlatform(BusstopPlatform *platform) const {
    return platform = defaultPlatform();
}

void Busstop::setDefaultPlatform(BusstopPlatform *platform) {
    if(!platform || !_data.platforms.contains(platform->id()))
        return;

    _data.defaultPlatform = platform;
}

QJsonObject Busstop::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("name",  name());
    jsonObject.insert("flags", flags().toInt());

    QJsonArray jsonPlatforms;
    for(BusstopPlatform *current : _data.platforms)
        jsonPlatforms.append(current->toJson());

    jsonObject.insert("platforms", jsonPlatforms);
    BusstopPlatform *defaultPlatform = this->defaultPlatform();
    jsonObject.insert("defaultPlatform", defaultPlatform ? defaultPlatform->idAsString() : "");
    return jsonObject;
}

void Busstop::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
    _data.name  = jsonObject.value("name").toString(tr("Unnamed busstop"));
    _data.flags = BusstopFlags::fromInt(jsonObject.value("flags").toInt(0));

    QUuid defaultPlatformID = QUuid::fromString(jsonObject.value("defaultPlatform").toString());

    QJsonArray jsonPlatforms = jsonObject.value("platforms").toArray();
    for(QJsonValue val : jsonPlatforms) {
        QJsonObject obj = val.toObject();
        BusstopPlatform *bp = new BusstopPlatform(this, obj);
        _data.platforms.insert(bp->id(), bp);
        if(bp->id() == defaultPlatformID)
            _data.defaultPlatform = bp;
    }
}
