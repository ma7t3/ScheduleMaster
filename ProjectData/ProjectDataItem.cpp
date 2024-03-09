#include "ProjectDataItem.h"

#include "App/global.h"

ProjectDataItem::ProjectDataItem(QObject *parent, const QString &id) :
    QObject(parent),
    _id(id) {
    if(this->id() == "")
        _id = global::getNewID();
}

ProjectDataItem::ProjectDataItem(QObject *parent, const QJsonObject &jsonObject) :
    QObject(parent) {
    fromJson(jsonObject);
}

ProjectDataItem::ProjectDataItem(const ProjectDataItem &other) : QObject(other.parent()) {
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

void ProjectDataItem::fromJson(const QJsonObject &jsonObject) {
    if(!jsonObject.contains("id") || !jsonObject.find("id")->isString())
        _id = global::getNewID();
    else
        _id = jsonObject.value("id").toString(global::getNewID());
}

QJsonObject ProjectDataItem::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("id", _id);
    return jsonObject;
}

void ProjectDataItem::refreshChilds() {
    //
}
