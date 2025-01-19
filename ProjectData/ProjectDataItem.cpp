#include "ProjectDataItem.h"

ProjectDataItem::ProjectDataItem(QObject *parent, const QUuid &id) :
    QObject(parent),
    _id(id.isNull() ? generateID() : id) {
}

ProjectDataItem::ProjectDataItem(QObject *parent, const QJsonObject &jsonObject) :
    QObject(parent) {
    fromJson(jsonObject);
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

QJsonObject ProjectDataItem::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("id", idAsString());
    return jsonObject;
}

void ProjectDataItem::fromJson(const QJsonObject &jsonObject) {
    QUuid id = QUuid::fromString(jsonObject.value("id").toString());
    _id = id.isNull() ? generateID() : id;
}
