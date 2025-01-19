#include "ProjectDataItem.h"

#include "App/global.h"

ProjectDataItem::ProjectDataItem(QObject *parent, const QString &id) :
    QObject(parent),
    _id(id),
    _inUse(false) {
    if(this->id().isEmpty())
        _id = getNewID();
}

ProjectDataItem::ProjectDataItem(QObject *parent, const QJsonObject &jsonObject) :
    QObject(parent),
    _inUse(false) {
    fromJson(jsonObject);
}

ProjectDataItem::ProjectDataItem(const ProjectDataItem &other) :
    QObject(other.parent()) {
    copy(other);
}

ProjectDataItem ProjectDataItem::operator=(const ProjectDataItem &other) {
    copy(other);
    return *this;
}

void ProjectDataItem::copy(const ProjectDataItem &other) {
    _id = other.id();
}

QString ProjectDataItem::id() const {
    return _id;
}

bool ProjectDataItem::inUse() const {
    return _inUse;
}

void ProjectDataItem::setInUse(const bool &newInUse) {
    _inUse = newInUse;
}

void ProjectDataItem::fromJson(const QJsonObject &jsonObject) {
    if(!jsonObject.contains("id") || !jsonObject.find("id")->isString())
        _id = getNewID();
    else
        _id = jsonObject.value("id").toString(getNewID());
}

QJsonObject ProjectDataItem::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("id", _id);
    return jsonObject;
}

QString ProjectDataItem::getNewID() {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}
