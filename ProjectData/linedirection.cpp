#include "linedirection.h"

LineDirection::LineDirection(const QString &id, const QString &description) :
    ProjectDataItem(id), _description(description) {}

LineDirection::LineDirection(const LineDirection &other) {
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

void LineDirection::setDescription(const QString &description) {
    _description = description;
}

QString LineDirection::description() const {
    return _description;
}

