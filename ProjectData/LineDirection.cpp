#include "LineDirection.h"

LineDirection::LineDirection(QObject *parent, const QUuid &id) :
    ProjectDataItem(parent, id) {
}

LineDirection::LineDirection(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

LineDirection::Data LineDirection::data() const {
    return _data;
}

void LineDirection::setData(const Data &newData) {
    _data = newData;
    emit changed();
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
    _data.description = jsonObject.value("description").toString(tr("unkown line direction"));
}

