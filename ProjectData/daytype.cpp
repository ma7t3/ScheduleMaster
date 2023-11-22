#include "daytype.h"

DayType::DayType(QString id) : AbstractProjectDataItem(id), WeekDays(MonFri) {}

DayType::DayType(QString id, QString name, int code) :
    AbstractProjectDataItem(id),
    WeekDays(code),
    m_name(name)
{}

DayType::DayType(QString id, QString name, bool monday, bool tuesday, bool wednesday, bool thursday, bool friday, bool saturday, bool sunday, bool holiday, bool school, bool noSchool) :
    AbstractProjectDataItem(id),
    WeekDays(monday, tuesday, wednesday, thursday, friday, saturday, sunday, holiday, school, noSchool),
    m_name(name)
{}

void DayType::setName(QString name) { m_name = name; }
QString DayType::name() { return m_name; }

void DayType::operator=(DayType other) {
    setName(other.name());

    setMonday(other.monday());
    setThursday(other.tuesday());
    setWednesday(other.wednesday());
    setThursday(other.thursday());
    setFriday(other.friday());
    setSaturday(other.saturday());
    setSunday(other.sunday());
    setHoliday(other.holiday());
    setSchool(other.school());
    setNoSchool(other.noSchool());
}
