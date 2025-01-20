#include "Line.h"


Line::Line(QObject *parent, const QUuid &id) :
    ProjectDataItem(parent, id) {
}

Line::Line(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

bool Line::operator<(const Line &other) const {
    return name() < other.name();
}

Line::Data Line::data() const {
    return _data;
}

void Line::setData(const Data &newData) {
    _data = newData;
    emit changed();
}

QString Line::name() const {
    return _data.name;
}

void Line::setName(const QString &newName) {
    _data.name = newName;
    emit changed();
}

QString Line::description() const {
    return _data.description;
}

void Line::setDescription(const QString &newDescription) {
    _data.description = newDescription;
}

QColor Line::color() const {
    return _data.color;
}

void Line::setColor(const QColor &newColor) {
    _data.color = newColor;
    emit changed();
}

QJsonObject Line::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    // TODO
    return jsonObject;
}

void Line::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
}

