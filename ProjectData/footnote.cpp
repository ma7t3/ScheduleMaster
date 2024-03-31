#include "footnote.h"

Footnote::Footnote(QObject *parent,
                   const QString &id,
                   const QString &identifier,
                   const QString &name,
                   const QString &description) :
    ProjectDataItem(parent, id),
    _identifier(identifier), _name(name), _description(description),
    _autoAssignWeekDaysEnabled(false) {}

}

Footnote::Footnote(const Footnote &other) :
    ProjectDataItem(other.parent()) {
    copy(other);
}

Footnote Footnote::operator=(const Footnote &other) {
    copy(other);
    return *this;
}

bool Footnote::operator<(const Footnote &other) {
    return name() < other.name();
}

void Footnote::copy(const Footnote &other) {
    ProjectDataItem::copy(other);
    setIdentifier(other.identifier());
    setName(other.name());
    setDescription(other.description());
WeekDays Footnote::autoAssignCareWeekDays() const {
    return _autoAssignCareWeekDays;
}

void Footnote::setAutoAssignCareWeekDays(const WeekDays &newAutoAssignCareWeekDays) {
    _autoAssignCareWeekDays = newAutoAssignCareWeekDays;
}

WeekDays Footnote::autoAssignWeekDays() const {
    return _autoAssignWeekDays;
}

void Footnote::setAutoAssignWeekDays(const WeekDays &newAutoAssignWeekDays) {
    _autoAssignWeekDays = newAutoAssignWeekDays;
}

bool Footnote::autoAssignWeekDaysEnabled() const {
    return _autoAssignWeekDaysEnabled;
}

void Footnote::setAutoAssignWeekDaysEnabled(bool newAutoAssignWeekDaysEnabled) {
    _autoAssignWeekDaysEnabled = newAutoAssignWeekDaysEnabled;
}

QString Footnote::identifier() const {
    return _identifier;
}

void Footnote::setIdentifier(const QString &newIdentifier) {
    _identifier = newIdentifier;
}

QString Footnote::name() const {
    return _name;
}

void Footnote::setName(const QString &newName) {
    _name = newName;
}

QString Footnote::description() const {
    return _description;
}

void Footnote::setDescription(const QString &newDescription) {
    _description = newDescription;
}


