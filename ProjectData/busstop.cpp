#include "ProjectData/busstop.h"

Busstop::Busstop(const QString &id, const QString &name, const bool &important) :
    ProjectDataItem(id), _name(name), _important(important) {}


Busstop::Busstop(const Busstop &other) {
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
