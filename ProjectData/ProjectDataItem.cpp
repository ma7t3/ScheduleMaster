#include "ProjectDataItem.h"

#include "App/global.h"

ProjectDataItem::ProjectDataItem(const QString &id) :
    QObject(nullptr),
    _id(id) {
}


ProjectDataItem::ProjectDataItem(const QJsonObject &jsonObject) :
    QObject(nullptr) {
    fromJson(jsonObject);
}

ProjectDataItem::ProjectDataItem(const ProjectDataItem &other) : QObject(nullptr) {
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

ProjectDataItem ProjectDataItem::fromJson(const QJsonObject &jsonObject) {
    if(!jsonObject.contains("id") || !jsonObject.find("id")->isString())
        _id = global::getNewID();
    else
        _id = jsonObject.value("id").toString();

    return *this;
}

QJsonObject ProjectDataItem::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("id", _id);
    return jsonObject;
}
