#include "Busstop.h"

Busstop::Busstop(QObject *parent, const QUuid &id) :
    ProjectDataItem(parent, id) {
}

Busstop::Busstop(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

bool Busstop::operator<(const Busstop &other) {
    return name() < other.name();
}

Busstop::Data Busstop::data() const {
    return _data;
}

void Busstop::setData(const Data &newData) {
    _data = newData;
    emit changed();
}

QString Busstop::name() const {
    return _data.name;
}

void Busstop::setName(const QString &newName) {
    _data.name = newName;
    emit changed();
}

bool Busstop::important() const {
    return _data.important;
}

void Busstop::setImportant(const bool &newImportant) {
    _data.important = newImportant;
    emit changed();
}

QJsonObject Busstop::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("name",      name());
    jsonObject.insert("important", important());
    return jsonObject;
}

void Busstop::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
    _data.name      = jsonObject.value("name").toString(tr("Unnamed busstop"));
    _data.important = jsonObject.value("important").toBool(false);
}
