#include "ProjectData\weekdays.h"

WeekDays::WeekDays() : ProjectDataItem("") {
    setCode(995);
}

WeekDays::WeekDays(const int &code) : ProjectDataItem("") {
    setCode(code);
}

WeekDays::WeekDays(const bool &monday,
                   const bool &tuesday,
                   const bool &wednesday,
                   const bool &thursday,
                   const bool &friday,
                   const bool &saturday,
                   const bool &sunday,
                   const bool &holiday,
                   const bool &school,
                   const bool &vacation) :
    ProjectDataItem("") {

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

WeekDays::WeekDays(const WeekDays &other) {
    copy(other);
}

bool WeekDays::operator ==(const WeekDays &w) const {
    return toCode() == w.toCode();
}

bool WeekDays::operator ==(WeekDays *w) const {
    return toCode() == w->toCode();
}

bool WeekDays::operator <=(const WeekDays &w) const {
    return isIn(w);
}

bool WeekDays::operator <=(WeekDays *w) const {
    return isIn(*w);
}

WeekDays WeekDays::operator=(const WeekDays &other) {
    copy(other);
    return *this;
}

void WeekDays::copy(const WeekDays &other) {
    ProjectDataItem::copy(other);
    setDay(WeekDay::monday,    other.day(WeekDay::monday));
    setDay(WeekDay::tuesday,   other.day(WeekDay::tuesday));
    setDay(WeekDay::wednesday, other.day(WeekDay::wednesday));
    setDay(WeekDay::thursday,  other.day(WeekDay::thursday));
    setDay(WeekDay::friday,    other.day(WeekDay::friday));
    setDay(WeekDay::saturday,  other.day(WeekDay::saturday));
    setDay(WeekDay::sunday,    other.day(WeekDay::sunday));
    setDay(WeekDay::holiday,   other.day(WeekDay::holiday));
    setDay(WeekDay::school,    other.day(WeekDay::school));
    setDay(WeekDay::vacation,  other.day(WeekDay::vacation));
}


bool WeekDays::day(const WeekDay &day) const {
    /*qDebug() << _days.at(day);
    qDebug() << _days.count(WeekDay::monday);*/
    return _days.at(day);
}

void WeekDays::setDay(const WeekDay &day, const bool &value) {
    _days[day] = value;
}

void WeekDays::setCode(const int &code) {
    QString bin = QString::number(code, 2);

    while(bin.length() < 10)
        bin = "0" + bin;

    setDay(WeekDay::monday,    bin[0] == '1' ? true : false);
    setDay(WeekDay::tuesday,   bin[1] == '1' ? true : false);
    setDay(WeekDay::wednesday, bin[2] == '1' ? true : false);
    setDay(WeekDay::thursday,  bin[3] == '1' ? true : false);
    setDay(WeekDay::friday,    bin[4] == '1' ? true : false);
    setDay(WeekDay::saturday,  bin[5] == '1' ? true : false);
    setDay(WeekDay::sunday,    bin[6] == '1' ? true : false);
    setDay(WeekDay::holiday,   bin[7] == '1' ? true : false);
    setDay(WeekDay::school,    bin[8] == '1' ? true : false);
    setDay(WeekDay::vacation,  bin[9] == '1' ? true : false);
}

int WeekDays::toCode() const {
    QString bin = "";

    bin += day(WeekDay::monday)    ? "1" : "0";
    bin += day(WeekDay::tuesday)   ? "1" : "0";
    bin += day(WeekDay::wednesday) ? "1" : "0";
    bin += day(WeekDay::thursday)  ? "1" : "0";
    bin += day(WeekDay::friday)    ? "1" : "0";
    bin += day(WeekDay::saturday)  ? "1" : "0";
    bin += day(WeekDay::sunday)    ? "1" : "0";
    bin += day(WeekDay::holiday)   ? "1" : "0";
    bin += day(WeekDay::school)    ? "1" : "0";
    bin += day(WeekDay::vacation)  ? "1" : "0";

    bool ok;
    int r = bin.toInt(&ok, 2);
    return r;
}

QString WeekDays::toString() const {
    return "";
}

WeekDays WeekDays::shfitedToNextDay() const {
    if(day(WeekDay::sunday) != day(WeekDay::holiday))
        return WeekDays(
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
        return WeekDays(
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

    if(list.count() == 0)
        return WeekDays(0);

    WeekDays result = list[0];

    for(int i = 1; i < list.count(); i++) {
        WeekDays w = list[i];

        result.setDay(WeekDay::monday,    (result.day(WeekDay::monday)    || w.day(WeekDay::monday)));
        result.setDay(WeekDay::tuesday,    (result.day(WeekDay::tuesday)   || w.day(WeekDay::tuesday)));
        result.setDay(WeekDay::wednesday,   (result.day(WeekDay::wednesday) || w.day(WeekDay::wednesday)));
        result.setDay(WeekDay::thursday, (result.day(WeekDay::thursday)  || w.day(WeekDay::thursday)));
        result.setDay(WeekDay::friday,  (result.day(WeekDay::friday)    || w.day(WeekDay::friday)));
        result.setDay(WeekDay::saturday,    (result.day(WeekDay::saturday)  || w.day(WeekDay::saturday)));
        result.setDay(WeekDay::sunday,  (result.day(WeekDay::sunday)    || w.day(WeekDay::sunday)));
        result.setDay(WeekDay::holiday,   (result.day(WeekDay::holiday)   || w.day(WeekDay::holiday)));
        result.setDay(WeekDay::school,    (result.day(WeekDay::school)    || w.day(WeekDay::school)));
        result.setDay(WeekDay::vacation,  (result.day(WeekDay::vacation)  || w.day(WeekDay::vacation)));
    }

    return result;
}

bool WeekDays::overlap(const QList<WeekDays> &list) {
    if(list.count() == 0)
        return false;

    WeekDays sum = list[0];

    for(int i = 1; i < list.count(); i++) {
        WeekDays w = list[i];

        if(!(sum.day(WeekDay::school) && w.day(WeekDay::school)) && !(sum.day(WeekDay::vacation) && w.day(WeekDay::vacation))) {
            sum = combine({sum, w});
            continue;
        }

        bool result = false;
        result = !result && (sum.day(WeekDay::monday)    && w.day(WeekDay::monday))    ? true : result;
        result = !result && (sum.day(WeekDay::tuesday)   && w.day(WeekDay::tuesday))   ? true : result;
        result = !result && (sum.day(WeekDay::wednesday) && w.day(WeekDay::wednesday)) ? true : result;
        result = !result && (sum.day(WeekDay::thursday)  && w.day(WeekDay::thursday))  ? true : result;
        result = !result && (sum.day(WeekDay::friday)    && w.day(WeekDay::friday))    ? true : result;
        result = !result && (sum.day(WeekDay::saturday)  && w.day(WeekDay::saturday))  ? true : result;
        result = !result && (sum.day(WeekDay::sunday)    && w.day(WeekDay::sunday))    ? true : result;
        result = !result && (sum.day(WeekDay::holiday)   && w.day(WeekDay::holiday))   ? true : result;

        if(result)
            return true;

        sum = combine({sum, w});
        continue;
    }
    return false;
}

WeekDays WeekDays::intersection(const QList<WeekDays> &list) {
    if(list.count() == 0)
        return WeekDays(0);

    WeekDays result(0);
    for(int i = 0; i < list.count(); i++) {
        WeekDays w1 = list[i];

        for(int j = 0; j < list.count(); j++) {
            if(i == j)
                continue;

            WeekDays w2 = list[j];
            WeekDays r(0);

            r.setDay(WeekDay::monday,    w1.day(WeekDay::monday)    && w2.day(WeekDay::monday));
            r.setDay(WeekDay::tuesday,   w1.day(WeekDay::tuesday)   && w2.day(WeekDay::tuesday));
            r.setDay(WeekDay::wednesday, w1.day(WeekDay::wednesday) && w2.day(WeekDay::wednesday));
            r.setDay(WeekDay::thursday,  w1.day(WeekDay::thursday)  && w2.day(WeekDay::thursday));
            r.setDay(WeekDay::friday,    w1.day(WeekDay::friday)    && w2.day(WeekDay::friday));
            r.setDay(WeekDay::saturday,  w1.day(WeekDay::saturday)  && w2.day(WeekDay::saturday));
            r.setDay(WeekDay::sunday,    w1.day(WeekDay::sunday)    && w2.day(WeekDay::sunday));
            r.setDay(WeekDay::holiday,   w1.day(WeekDay::holiday)   && w2.day(WeekDay::holiday));
            r.setDay(WeekDay::school,    w1.day(WeekDay::school)    && w2.day(WeekDay::school));
            r.setDay(WeekDay::vacation,  w1.day(WeekDay::vacation)  && w2.day(WeekDay::vacation));

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
