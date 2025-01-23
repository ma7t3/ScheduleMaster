#include "ProjectDataItem.h"

ProjectDataItem::ProjectDataItem(QObject *parent, const QUuid &id) :
    QObject(parent),
    _id(id.isNull() ? generateID() : id),
    _inUse(false) {
}

QUuid ProjectDataItem::id() const {
    return _id;
}

QString ProjectDataItem::idAsString() const {
    return _id.toString(QUuid::WithoutBraces);
}

QUuid ProjectDataItem::generateID() {
    return QUuid::createUuid();
}

bool ProjectDataItem::inUse() const {
    return _inUse;
}

void ProjectDataItem::setInUse(const bool &newInUse) {
    _inUse = newInUse;
}

QJsonObject ProjectDataItem::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("id", idAsString());
    return jsonObject;
}

void ProjectDataItem::fromJson(const QJsonObject &jsonObject) {
    QUuid id = QUuid::fromString(jsonObject.value("id").toString());
    _id = id.isNull() ? generateID() : id;
}

void ProjectDataItem::setID(const QUuid &newID) {
    _id = newID;
    QObject::setObjectName(idAsString());
}
