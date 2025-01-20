#include "BusstopPlatform.h"

BusstopPlatform::BusstopPlatform(QObject *parent, const QUuid &id) :
    ProjectDataItem(parent, id) {
}

BusstopPlatform::BusstopPlatform(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

bool BusstopPlatform::operator<(const BusstopPlatform &other) const {
    return name() < other.name();
}

BusstopPlatform::Data BusstopPlatform::data() const {
    return _data;
}

void BusstopPlatform::setData(const Data &newData) {
    _data = newData;
}

QString BusstopPlatform::name() const {
    return _data.name;
}

void BusstopPlatform::setName(const QString &newName) {
    _data.name = newName;
    emit changed();
}

BusstopPlatformFlags BusstopPlatform::flags() const {
    return _data.flags;
}

void BusstopPlatform::setFlags(const BusstopPlatformFlags &newFlags) {
    _data.flags = newFlags;
    emit changed();
}

QJsonObject BusstopPlatform::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("name",  _data.name);
    jsonObject.insert("flags", _data.flags.toInt());
    return jsonObject;
}

void BusstopPlatform::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
    _data.name  = jsonObject.value("name").toString(tr("unnamed busstop platform"));
    _data.flags = BusstopPlatformFlags::fromInt(jsonObject.value("flags").toInt());
}
