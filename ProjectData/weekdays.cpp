#include "ProjectData\weekdays.h"

WeekDays::WeekDays() { setCode(MonFri); }

WeekDays::WeekDays(int code) { setCode(code); }

WeekDays::WeekDays(bool monday, bool tuesday, bool wednesday, bool thursday, bool friday, bool saturday, bool sunday, bool holiday, bool school, bool noSchool) :
    m_monday(monday),
    m_tuesday(tuesday),
    m_wednesday(wednesday),
    m_thursday(thursday),
    m_friday(friday),
    m_saturday(saturday),
    m_sunday(sunday),
    m_holiday(holiday),
    m_school(school),
    m_noSchool(noSchool)
{}

void WeekDays::setMonday(bool b)      {m_monday = b;}
void WeekDays::setTuesday(bool b)     {m_tuesday = b;}
void WeekDays::setWednesday(bool b)   {m_wednesday = b;}
void WeekDays::setThursday(bool b)    {m_thursday = b;}
void WeekDays::setFriday(bool b)      {m_friday = b;}
void WeekDays::setSaturday(bool b)    {m_saturday = b;}
void WeekDays::setSunday(bool b)      {m_sunday = b;}
void WeekDays::setHoliday(bool b)     {m_holiday = b;}
void WeekDays::setSchool(bool b)      {m_school = b;}
void WeekDays::setNoSchool(bool b)    {m_noSchool = b;}
bool WeekDays::monday()            {return m_monday;}
bool WeekDays::tuesday()           {return m_tuesday;}
bool WeekDays::wednesday()         {return m_wednesday;}
bool WeekDays::thursday()          {return m_thursday;}
bool WeekDays::friday()            {return m_friday;}
bool WeekDays::saturday()          {return m_saturday;}
bool WeekDays::sunday()            {return m_sunday;}
bool WeekDays::holiday()           {return m_holiday;}
bool WeekDays::school()            {return m_school;}
bool WeekDays::noSchool()          {return m_noSchool;}

void WeekDays::setCode(int code) {
    QString bin = QString::number(code, 2);

    while(bin.length() < 10)
        bin = "0" + bin;

    m_monday    = bin[0] == '1' ? true : false;
    m_tuesday   = bin[1] == '1' ? true : false;
    m_wednesday = bin[2] == '1' ? true : false;
    m_thursday  = bin[3] == '1' ? true : false;
    m_friday    = bin[4] == '1' ? true : false;
    m_saturday  = bin[5] == '1' ? true : false;
    m_sunday    = bin[6] == '1' ? true : false;
    m_holiday   = bin[7] == '1' ? true : false;
    m_school    = bin[8] == '1' ? true : false;
    m_noSchool  = bin[9] == '1' ? true : false;
}

int WeekDays::toCode() {
    QString bin = "";

    bin += m_monday       ? "1" : "0";
    bin += m_tuesday      ? "1" : "0";
    bin += m_wednesday    ? "1" : "0";
    bin += m_thursday     ? "1" : "0";
    bin += m_friday       ? "1" : "0";
    bin += m_saturday     ? "1" : "0";
    bin += m_sunday       ? "1" : "0";
    bin += m_holiday      ? "1" : "0";
    bin += m_school       ? "1" : "0";
    bin += m_noSchool     ? "1" : "0";

    bool ok;
    int r = bin.toInt(&ok, 2);
    return r;
}

QString WeekDays::toString() {
    QString result = "";

    QList<bool> list = {m_monday, m_tuesday, m_wednesday, m_thursday, m_friday, m_saturday, m_sunday};
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
    if(m_sunday != m_holiday)
        return WeekDays(m_sunday, m_monday, m_tuesday, m_wednesday, m_thursday, m_friday, m_saturday, m_holiday, m_school, m_noSchool);
    else
        return WeekDays(m_sunday, m_monday, m_tuesday, m_wednesday, m_thursday, m_friday, m_saturday, m_saturday, m_school, m_noSchool);
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

    if(noSchool() && !w.noSchool())
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
        result.setNoSchool  (result.noSchool()  || w.noSchool());
    }

    return result;
}

bool WeekDays::overlap(QList<WeekDays> list) {
    if(list.count() == 0)
        return false;

    WeekDays sum = list[0];

    for(int i = 1; i < list.count(); i++) {
        WeekDays w = list[i];

        if(!(sum.school() && w.school()) && !(sum.noSchool() && w.noSchool())) {
            sum = *new WeekDays(combine({sum, w}));
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

        sum = *new WeekDays(combine({sum, w}));
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
            r.setNoSchool(w1.noSchool() && w2.noSchool());
            result = *new WeekDays(combine(result, r));
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

void WeekDays::operator=(WeekDays other) {
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


