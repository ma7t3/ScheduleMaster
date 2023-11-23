#include "linedirection.h"

LineDirection::LineDirection(QString id, QString description) :
    AbstractProjectDataItem(id),
    m_description(description)
{}

void LineDirection::setDescription(QString description) { m_description = description; }
QString LineDirection::description()                    { return m_description; }

void LineDirection::overwrite(LineDirection &other) {
    setDescription(other.description());
}

