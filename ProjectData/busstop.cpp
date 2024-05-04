#include "ProjectData/busstop.h"

Busstop::Busstop(QObject *parent, const QString &id) :
    ProjectDataItem(parent, id), _important(false) {}

Busstop::Busstop(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent), _important(false) {
    fromJson(jsonObject);
}

Busstop::Busstop(const Busstop &other) :
    ProjectDataItem(other.parent()) {
    copy(other);
}

Busstop Busstop::operator=(const Busstop &other) {
    copy(other);
    return *this;
}

bool Busstop::operator<(const Busstop &other) {
    return name() < other.name();
}

void Busstop::copy(const Busstop &other) {
    ProjectDataItem::copy(other);
    setName(other.name());
    setImportant(other.important());
}

void Busstop::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);

    setName(jsonObject.value("name").isString() ? jsonObject.value("name").toString() : tr("Unnamed Busstop"));
    setImportant(jsonObject.value("important").isBool() ? jsonObject.value("important").toBool() : false);
}

QJsonObject Busstop::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("name", name());
    jsonObject.insert("important", important());
    return jsonObject;
}

QString Busstop::name() const {
    return _name;
}

void Busstop::setName(const QString &name) {
    _name = name;
}

bool Busstop::isImportant() const {
    return _important;
}

bool Busstop::important() const {
    return isImportant();
}

void Busstop::setImportant(const bool &important) {
    _important = important;
}
