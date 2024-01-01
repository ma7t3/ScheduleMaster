#include "daytype.h"

DayType::DayType(QString id) : AbstractProjectDataItem(id), WeekDays(MonFri) {}

DayType::DayType(QString id, QString name, int code) :
    AbstractProjectDataItem(id),
    WeekDays(code),
    _name(name)
{}

DayType::DayType(QString id, QString name, bool monday, bool tuesday, bool wednesday, bool thursday, bool friday, bool saturday, bool sunday, bool holiday, bool school, bool noSchool) :
    AbstractProjectDataItem(id),
    WeekDays(monday, tuesday, wednesday, thursday, friday, saturday, sunday, holiday, school, noSchool),
    _name(name)
{}

void DayType::setName(QString name) { _name = name; }
QString DayType::name() { return _name; }

void DayType::overwrite(DayType &other) {
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
