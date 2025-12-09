#include "BusstopPlatform.h"

BusstopPlatform::BusstopPlatform(QObject *parent, const QUuid &id, BusstopPlatform *original) :
    ProjectDataItemCRTP(parent, id, original) {
}

BusstopPlatform::BusstopPlatform(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItemCRTP(parent) {
    fromJson(jsonObject);
}

bool BusstopPlatform::operator<(const BusstopPlatform &other) const {
    return name() < other.name();
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
    QJsonObject jsonObject = ProjectDataItemCRTP::toJson();
    jsonObject.insert("name",  name());
    jsonObject.insert("flags", flags().toInt());
    return jsonObject;
}

void BusstopPlatform::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItemCRTP::fromJson(jsonObject);
    setName(jsonObject.value("name").toString(tr("unnamed busstop platform")));
    setFlags(BusstopPlatformFlags::fromInt(jsonObject.value("flags").toInt()));
}
