#include "ProjectData/busstop.h"

Busstop::Busstop(QString id, QString name, bool important) :
    AbstractProjectDataItem(id),
    _name(name),
    _important(important)

{}

QString Busstop::name() { return _name; }

bool Busstop::isImportant()
{
    return _important;
}

bool Busstop::important() { return isImportant(); }

void Busstop::setName(QString name) { _name = name; }

void Busstop::setImportant(bool important) { _important = important; }

void Busstop::overwrite(Busstop &other) {
    setName(other.name());
    setImportant(other.important());
}

bool Busstop::operator<(Busstop &other) {
    return name() < other.name();
}
