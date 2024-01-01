#include "linedirection.h"

LineDirection::LineDirection(QString id, QString description) :
    AbstractProjectDataItem(id),
    _description(description)
{}

void LineDirection::setDescription(QString description) { _description = description; }
QString LineDirection::description()                    { return _description; }

void LineDirection::overwrite(LineDirection &other) {
    setDescription(other.description());
}

