#include "ProjectData\weekdays.h"

WeekDays::WeekDays(QObject *parent) : ProjectDataItem(parent) {
    setCode(995);
}

WeekDays::WeekDays(QObject *parent, const int &code) : ProjectDataItem(parent) {
    setCode(code);
}

WeekDays::WeekDays(QObject *parent, const QString &id) :
    ProjectDataItem(parent, id) {
}

WeekDays::WeekDays(QObject *parent,
                   const bool &monday,
                   const bool &tuesday,
                   const bool &wednesday,
                   const bool &thursday,
                   const bool &friday,
                   const bool &saturday,
                   const bool &sunday,
                   const bool &holiday,
                   const bool &school,
                   const bool &vacation) :
    ProjectDataItem(parent) {

    setDay(WeekDay::monday, monday);
    setDay(WeekDay::tuesday, tuesday);
    setDay(WeekDay::wednesday, wednesday);
    setDay(WeekDay::thursday, thursday);
    setDay(WeekDay::friday, friday);
    setDay(WeekDay::saturday, saturday);
    setDay(WeekDay::sunday, sunday);
    setDay(WeekDay::holiday, holiday);
    setDay(WeekDay::school, school);
    setDay(WeekDay::vacation, vacation);
}

WeekDays::WeekDays(const WeekDays &other) :
    ProjectDataItem(other.parent()) {
    copy(other);
}

bool WeekDays::operator ==(const WeekDays &w) const {
    return toCode() == w.toCode();
}

bool WeekDays::operator!=(const WeekDays &w) const {
    return !(*this == w);
}

bool WeekDays::operator <=(const WeekDays &w) const {
    return isIn(w);
}

bool WeekDays::operator<(const WeekDays &other) const {
    return toCode() > other.toCode();
}

WeekDays WeekDays::operator=(const WeekDays &other) {
    copy(other);
    return *this;
}

void WeekDays::copy(const WeekDays &other) {
    ProjectDataItem::copy(other);
    _days = other._days;
}

void WeekDays::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
}

QJsonObject WeekDays::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("code", toCode());
    return jsonObject;
}

bool WeekDays::day(const WeekDaysFlag &day) const {
    return _days & day;
}

bool WeekDays::dayAtIndex(const int &index) const {
    WeekDay w;
    switch(index) {
        case 0: w = WeekDay(0x001); break;
        case 1: w = WeekDay(0x002); break;
        case 2: w = WeekDay(0x004); break;
        case 3: w = WeekDay(0x008); break;
        case 4: w = WeekDay(0x010); break;
        case 5: w = WeekDay(0x020); break;
        case 6: w = WeekDay(0x040); break;
        case 7: w = WeekDay(0x080); break;
        case 8: w = WeekDay(0x100); break;
        case 9: w = WeekDay(0x200); break;
    }

    return day(w);
}

void WeekDays::setDay(const WeekDaysFlag &day, const bool &value) {
    if(value)
        _days |= day;
    else
        _days &= ~day;
}

void WeekDays::setDayAtIndex(const int &index, const bool &value) {
    WeekDay w;
    switch(index) {
    case 0: w = WeekDay(0x001); break;
    case 1: w = WeekDay(0x002); break;
    case 2: w = WeekDay(0x004); break;
    case 3: w = WeekDay(0x008); break;
    case 4: w = WeekDay(0x010); break;
    case 5: w = WeekDay(0x020); break;
    case 6: w = WeekDay(0x040); break;
    case 7: w = WeekDay(0x080); break;
    case 8: w = WeekDay(0x100); break;
    case 9: w = WeekDay(0x200); break;
    }

    setDay(w, value);
}
void WeekDays::setCode(const int &code) {

    // use this code to convert from old file format
    /*unsigned int reverse_num = 0;
    for (unsigned int i = 0; i < 10; i++) {
        if (code & (1 << i)) {
            reverse_num |= 1 << (9 - i);
        }
    }
    _days = QFlags<WeekDay>::fromInt(reverse_num);*/

    _days = QFlags<WeekDay>::fromInt(code);
}

int WeekDays::toCode() const {
    return _days.toInt();
}

QString WeekDays::toString() const {
    QStringList result;
    QString currentStr;

    QStringList weekDayNames = {
        tr("Monday"),
        tr("Tuesday"),
        tr("Wednesday"),
        tr("Thursday"),
        tr("Friday"),
        tr("Saturday"),
        tr("Sunday")
    };

    for(int i = 0; i < 7; i++) {
        bool previous = i > 0 ? dayAtIndex(i - 1) : false;
        bool current  = dayAtIndex(i);
        bool next     = i < 6 ? dayAtIndex(i + 1) : false;

        if(!previous && current && next)
            currentStr = weekDayNames[i] + "-";

        if(!previous && current && !next)
            result << weekDayNames[i];

        if(previous && current && !next)
            result << (currentStr += weekDayNames[i]);
    }

    QString resultStr = result.join(", ");

    if(day(holiday))
        resultStr += tr(", Holidays");

    if(day(school) && !day(vacation))
        resultStr += tr(", School");

    if(!day(school) && day(vacation))
        resultStr += tr(", Vacation");

    return resultStr;
}

WeekDays WeekDays::shfitedToNextDay() const {
    if(day(WeekDay::sunday) != day(WeekDay::holiday))
        return WeekDays(parent(),
            day(WeekDay::sunday),
            day(WeekDay::monday),
            day(WeekDay::tuesday),
            day(WeekDay::wednesday),
            day(WeekDay::thursday),
            day(WeekDay::friday),
            day(WeekDay::saturday),
            day(WeekDay::holiday),
            day(WeekDay::school),
            day(WeekDay::vacation)
        );
    else
        return WeekDays(parent(),
            day(WeekDay::sunday),
            day(WeekDay::monday),
            day(WeekDay::tuesday),
            day(WeekDay::wednesday),
            day(WeekDay::thursday),
            day(WeekDay::friday),
            day(WeekDay::saturday),
            day(WeekDay::saturday),
            day(WeekDay::school),
            day(WeekDay::vacation)
        );
}


bool WeekDays::isIn(const WeekDays &w) const {
    if(day(WeekDay::monday) && !w.day(WeekDay::monday))
        return false;
    if(day(WeekDay::tuesday) && !w.day(WeekDay::tuesday))
        return false;
    if(day(WeekDay::wednesday) && !w.day(WeekDay::wednesday))
        return false;
    if(day(WeekDay::thursday) && !w.day(WeekDay::thursday))
        return false;
    if(day(WeekDay::friday) && !w.day(WeekDay::friday))
        return false;
    if(day(WeekDay::saturday) && !w.day(WeekDay::saturday))
        return false;
    if(day(WeekDay::sunday) && !w.day(WeekDay::sunday))
        return false;
    if(day(WeekDay::holiday) && !w.day(WeekDay::holiday))
        return false;
    if(day(WeekDay::school) && !w.day(WeekDay::school))
        return false;
    if(day(WeekDay::vacation) && !w.day(WeekDay::vacation))
        return false;

    return true;
}

WeekDays WeekDays::combine(const QList<WeekDays> &list) {

    if(list.isEmpty())
        return WeekDays(0);

    if(list.count() ==  1)
        return list[0];

    WeekDays result = list[0];

    for(int i = 1; i < list.count(); i++) {
        WeekDays w = list[i];

        for(int i = 0; i < 10; i++) {
            result.setDayAtIndex(i, (result.dayAtIndex(i) || w.dayAtIndex(i)));
        }
    }

    return result;
}

bool WeekDays::overlap(const QList<WeekDays> &list) {
    if(list.isEmpty() || list.count() ==  1)
        return false;

    /*
     * 0-7  School
     * 8-15 Vacation
     */

    int matches[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    for(int i = 0; i < list.count(); i++) {
        for(int j = 0; j < 8; j++) {
            if(list[i].dayAtIndex(j)) {

                if(list[i].day(school))
                    matches[j]++;

                if(list[i].day(vacation))
                    matches[j + 8]++;

            }
        }
    }

    for(int i = 0; i < 10; i++) {
        if(matches[i] > 1)
            return true;
    }

    return false;
}

WeekDays WeekDays::intersection(const QList<WeekDays> &list) {
    if(list.count() < 2)
        return WeekDays(0);

    WeekDays result(0);
    for(int i = 0; i < list.count(); i++) {
        WeekDays w1 = list[i];

        for(int j = 0; j < list.count(); j++) {
            if(i == j)
                continue;

            WeekDays w2 = list[j];
            WeekDays r(nullptr, 0);

            r.setDay(monday,    w1.day(monday)    && w2.day(monday));
            r.setDay(tuesday,   w1.day(tuesday)   && w2.day(tuesday));
            r.setDay(wednesday, w1.day(wednesday) && w2.day(wednesday));
            r.setDay(thursday,  w1.day(thursday)  && w2.day(thursday));
            r.setDay(friday,    w1.day(friday)    && w2.day(friday));
            r.setDay(saturday,  w1.day(saturday)  && w2.day(saturday));
            r.setDay(sunday,    w1.day(sunday)    && w2.day(sunday));
            r.setDay(holiday,   w1.day(holiday)   && w2.day(holiday));
            r.setDay(school,    w1.day(school)    && w2.day(school));
            r.setDay(vacation,  w1.day(vacation)  && w2.day(vacation));

            result = combine(result, r);
        }
    }

    return result;
}

WeekDays WeekDays::combine(const WeekDays &w1, const WeekDays  &w2) {
    return combine({w1, w2});
}

bool WeekDays::overlap(const WeekDays &w1, const WeekDays  &w2) {
    return overlap({w1, w2});
}

WeekDays WeekDays::intersection(const WeekDays &w1, const WeekDays  &w2) {
    return intersection({w1, w2});
}
