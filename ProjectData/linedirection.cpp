#include "linedirection.h"

LineDirection::LineDirection(QObject *parent, const QString &id) :
    ProjectDataItem(parent, id) {}

LineDirection::LineDirection(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

LineDirection::LineDirection(const LineDirection &other) :
    ProjectDataItem(other.parent()) {
    copy(other);
}

LineDirection LineDirection::operator=(const LineDirection &other) {
    copy(other);
    return *this;
}

void LineDirection::copy(const LineDirection &other) {
    ProjectDataItem::copy(other);
    setDescription(other.description());
}

void LineDirection::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
    setDescription(jsonObject.value("description").isString() ? jsonObject.value("description").toString() : tr("Unnamed Direction"));
}

QJsonObject LineDirection::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("description", description());
    return jsonObject;
}

void LineDirection::setDescription(const QString &description) {
    _description = description;
}

QString LineDirection::description() const {
    return _description;
}

