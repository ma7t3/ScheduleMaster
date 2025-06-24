#include "Line.h"

#include <QJsonArray>

Line::Line(QObject *parent, const QUuid &id, const bool &isClone) :
    ProjectDataItem(parent, id, isClone) {
}

Line::Line(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

bool Line::operator<(const Line &other) const {
    return name() < other.name();
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
    emit changed();
}

QColor Line::color() const {
    return _data.color;
}

void Line::setColor(const QColor &newColor) {
    _data.color = newColor;
    emit changed();
}

LineDirection *Line::createDirection(QObject *parent) {
    return new LineDirection(parent ? parent : this);
}

LineDirection *Line::createDirection(const QJsonObject &jsonObject) {
    return new LineDirection(this, jsonObject);
}

int Line::directionCount() const {
    return _data.directions.count();
}

LineDirection *Line::direction(const QUuid &id) const {
    return _data.directions.find(id);
}

PDIList<LineDirection> Line::directions() const {
    return _data.directions;
}

void Line::appendDirection(LineDirection *lineDirection) {
    _data.directions.append(lineDirection);
    emit changed();
}

void Line::insertDirection(const int &index, LineDirection *lineDirection) {
    if(index < 0 || index > _data.directions.count())
        return;

    _data.directions.insert(index, lineDirection);
    emit changed();
}

void Line::removeDirection(LineDirection *lineDirection) {
    _data.directions.remove(lineDirection);
    emit changed();
}

void Line::removeDirection(const QUuid &id) {
    _data.directions.remove(id);
    emit changed();
}

QJsonObject Line::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("name",        _data.name);
    jsonObject.insert("description", _data.description);
    jsonObject.insert("color",       _data.color.name());

    QJsonArray jsonDirections;
    for(LineDirection *current : _data.directions)
        jsonDirections.append(current->toJson());

    jsonObject.insert("directions", jsonDirections);
    return jsonObject;
}

void Line::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
    setName(jsonObject.value("name").toString(tr("Unnamed line")));
    setDescription(jsonObject.value("description").toString());
    setColor(QColor(jsonObject.value("color").toString()));
    QJsonArray jsonDirections = jsonObject.value("directions").toArray();
    for(QJsonValue val : jsonDirections)
        _data.directions.append(createDirection(val.toObject()));
}

