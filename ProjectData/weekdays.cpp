#include "ProjectData\weekdays.h"

WeekDays::WeekDays() { setCode(MonFri); }

WeekDays::WeekDays(int code) { setCode(code); }

WeekDays::WeekDays(bool monday, bool tuesday, bool wednesday, bool thursday, bool friday, bool saturday, bool sunday, bool holiday, bool school, bool vacation) :
    _monday(monday),
    _tuesday(tuesday),
    _wednesday(wednesday),
    _thursday(thursday),
    _friday(friday),
    _saturday(saturday),
    _sunday(sunday),
    _holiday(holiday),
    _school(school),
    _vacation(vacation)
{}

void WeekDays::setMonday(bool b)      {_monday = b;}
void WeekDays::setTuesday(bool b)     {_tuesday = b;}
void WeekDays::setWednesday(bool b)   {_wednesday = b;}
void WeekDays::setThursday(bool b)    {_thursday = b;}
void WeekDays::setFriday(bool b)      {_friday = b;}
void WeekDays::setSaturday(bool b)    {_saturday = b;}
void WeekDays::setSunday(bool b)      {_sunday = b;}
void WeekDays::setHoliday(bool b)     {_holiday = b;}
void WeekDays::setSchool(bool b)      {_school = b;}
void WeekDays::setVacation(bool b)    {_vacation = b;}
bool WeekDays::monday()            {return _monday;}
bool WeekDays::tuesday()           {return _tuesday;}
bool WeekDays::wednesday()         {return _wednesday;}
bool WeekDays::thursday()          {return _thursday;}
bool WeekDays::friday()            {return _friday;}
bool WeekDays::saturday()          {return _saturday;}
bool WeekDays::sunday()            {return _sunday;}
bool WeekDays::holiday()           {return _holiday;}
bool WeekDays::school()            {return _school;}
bool WeekDays::vacation()          {return _vacation;}

void WeekDays::setCode(int code) {
    QString bin = QString::number(code, 2);

    while(bin.length() < 10)
        bin = "0" + bin;

    _monday    = bin[0] == '1' ? true : false;
    _tuesday   = bin[1] == '1' ? true : false;
    _wednesday = bin[2] == '1' ? true : false;
    _thursday  = bin[3] == '1' ? true : false;
    _friday    = bin[4] == '1' ? true : false;
    _saturday  = bin[5] == '1' ? true : false;
    _sunday    = bin[6] == '1' ? true : false;
    _holiday   = bin[7] == '1' ? true : false;
    _school    = bin[8] == '1' ? true : false;
    _vacation  = bin[9] == '1' ? true : false;
}

int WeekDays::toCode() {
    QString bin = "";

    bin += _monday       ? "1" : "0";
    bin += _tuesday      ? "1" : "0";
    bin += _wednesday    ? "1" : "0";
    bin += _thursday     ? "1" : "0";
    bin += _friday       ? "1" : "0";
    bin += _saturday     ? "1" : "0";
    bin += _sunday       ? "1" : "0";
    bin += _holiday      ? "1" : "0";
    bin += _school       ? "1" : "0";
    bin += _vacation     ? "1" : "0";

    bool ok;
    int r = bin.toInt(&ok, 2);
    return r;
}

QString WeekDays::toString() {
    QString result = "";

    QList<bool> list = {_monday, _tuesday, _wednesday, _thursday, _friday, _saturday, _sunday};
    //QStringList names = {"monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday"};
    QStringList names = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

    bool lastTrue;
    bool firstTrue = false;
    for(int i = 0; i < list.count(); i++) {
        if(list[i]) {
            if(!firstTrue || i + 1 == list.count() || !list[i + 1]) {
                QString prefix;
                if(!firstTrue)
                    prefix = "";
                else if(lastTrue)
                    prefix = "-";
                else
                    prefix = ", ";

                result += (prefix + names[i]);
            }
        }
        lastTrue = list[i];
        firstTrue = lastTrue ? true : firstTrue;
    }

    return result;
}

WeekDays WeekDays::shfitedToNextDay() {
    if(_sunday != _holiday)
        return WeekDays(_sunday, _monday, _tuesday, _wednesday, _thursday, _friday, _saturday, _holiday, _school, _vacation);
    else
        return WeekDays(_sunday, _monday, _tuesday, _wednesday, _thursday, _friday, _saturday, _saturday, _school, _vacation);
}


bool WeekDays::isIn(WeekDays w) {
    if(monday() && !w.monday())
        return false;

    if(tuesday() && !w.tuesday())
        return false;

    if(wednesday() && !w.wednesday())
        return false;

    if(thursday() && !w.thursday())
        return false;

    if(friday() && !w.friday())
        return false;

    if(saturday() && !w.saturday())
        return false;

    if(sunday() && !w.sunday())
        return false;

    if(holiday() && !w.holiday())
        return false;

    if(school() && !w.school())
        return false;

    if(vacation() && !w.vacation())
        return false;

    return true;
}

WeekDays WeekDays::combine(QList<WeekDays> list) {

    if(list.count() == 0)
        return WeekDays(0);

    WeekDays result = list[0];

    for(int i = 1; i < list.count(); i++) {
        WeekDays w = list[i];

        result.setMonday    (result.monday()    || w.monday());
        result.setTuesday   (result.tuesday()   || w.tuesday());
        result.setWednesday (result.wednesday() || w.wednesday());
        result.setThursday  (result.thursday()  || w.thursday());
        result.setFriday    (result.friday()    || w.friday());
        result.setSaturday  (result.saturday()  || w.saturday());
        result.setSunday    (result.sunday()    || w.sunday());
        result.setHoliday   (result.holiday()   || w.holiday());
        result.setSchool    (result.school()    || w.school());
        result.setVacation  (result.vacation()  || w.vacation());
    }

    return result;
}

bool WeekDays::overlap(QList<WeekDays> list) {
    if(list.count() == 0)
        return false;

    WeekDays sum = list[0];

    for(int i = 1; i < list.count(); i++) {
        WeekDays w = list[i];

        if(!(sum.school() && w.school()) && !(sum.vacation() && w.vacation())) {
            sum = combine({sum, w});
            continue;
        }

        bool result = false;
        result = !result && (sum.monday()    && w.monday())    ? true : result;
        result = !result && (sum.tuesday()   && w.tuesday())   ? true : result;
        result = !result && (sum.wednesday() && w.wednesday()) ? true : result;
        result = !result && (sum.thursday()  && w.thursday())  ? true : result;
        result = !result && (sum.friday()    && w.friday())    ? true : result;
        result = !result && (sum.saturday()  && w.saturday())  ? true : result;
        result = !result && (sum.sunday()    && w.sunday())    ? true : result;
        result = !result && (sum.holiday()   && w.holiday())   ? true : result;

        if(result)
            return true;

        sum = combine({sum, w});
        continue;
    }
    return false;
}

WeekDays WeekDays::intersection(QList<WeekDays> list) {
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

            r.setMonday(w1.monday() && w2.monday());
            r.setTuesday(w1.tuesday() && w2.tuesday());
            r.setWednesday(w1.wednesday() && w2.wednesday());
            r.setThursday(w1.thursday() && w2.thursday());
            r.setFriday(w1.friday() && w2.friday());
            r.setSaturday(w1.saturday() && w2.saturday());
            r.setSunday(w1.sunday() && w2.sunday());
            r.setHoliday(w1.holiday() && w2.holiday());
            r.setSchool(w1.school() && w2.school());
            r.setVacation(w1.vacation() && w2.vacation());
            result = combine(result, r);
        }
    }

    return result;
}

WeekDays WeekDays::combine(WeekDays w1, WeekDays w2)      { return combine({w1, w2}); }
bool WeekDays::overlap(WeekDays w1, WeekDays w2)          { return overlap({w1, w2}); }
WeekDays WeekDays::intersection(WeekDays w1, WeekDays w2) { return intersection({w1, w2}); }

bool WeekDays::operator ==(WeekDays *w) { return toCode() == w->toCode(); }

bool WeekDays::operator <=(WeekDays *w) { return isIn(*w); }



bool WeekDays::operator ==(WeekDays w) { return toCode() == w.toCode(); }

bool WeekDays::operator <=(WeekDays w) { return isIn(w); }

void WeekDays::overwrite(WeekDays &other) {
    setMonday(other.monday());
    setThursday(other.tuesday());
    setWednesday(other.wednesday());
    setThursday(other.thursday());
    setFriday(other.friday());
    setSaturday(other.saturday());
    setSunday(other.sunday());
    setHoliday(other.holiday());
    setSchool(other.school());
    setVacation(other.vacation());
}


