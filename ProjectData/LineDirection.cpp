#include "LineDirection.h"

LineDirection::LineDirection(QObject *parent, const QUuid &id, const bool &isClone) :
    ProjectDataItem(parent, id, isClone) {
}

LineDirection::LineDirection(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

QString LineDirection::description() const {
    return _data.description;
}

void LineDirection::setDescription(const QString &newDescription) {
    _data.description = newDescription;
    emit changed();
}

QJsonObject LineDirection::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("description", _data.description);
    return jsonObject;
}

void LineDirection::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
    setDescription(jsonObject.value("description").toString(tr("unkown line direction")));
}

