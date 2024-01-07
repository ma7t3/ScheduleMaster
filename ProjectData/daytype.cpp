#include "daytype.h"

DayType::DayType(const QString &id) :
    ProjectDataItem(id),
    WeekDays(MonFri) {
}

DayType::DayType(const QString &id, const QString &name, const int &code) :
    ProjectDataItem(id),
    WeekDays(code),
    _name(name)
{}

DayType::DayType(const QString &id,
                 const QString &name,
                 const bool &monday,
                 const bool &tuesday,
                 const bool &wednesday,
                 const bool &thursday,
                 const bool &friday,
                 const bool &saturday,
                 const bool &sunday,
                 const bool &holiday,
                 const bool &school,
                 const bool &noSchool) :
    ProjectDataItem(id),
    WeekDays(
        monday, tuesday, wednesday, thursday, friday, saturday, sunday, holiday, school, noSchool),
    _name(name) {}

DayType::DayType(const DayType &other) {
    copy(other);
}

DayType DayType::operator=(const DayType &other) {
    copy(other);
    return *this;
}

void DayType::copy(const DayType &other) {
    WeekDays::copy(other);
    setName(other.name());
}

void DayType::setName(const QString &name) {
    _name = name;
}

QString DayType::name() const {
    return _name;
}




