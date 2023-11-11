#ifndef WEEKDAYS_H
#define WEEKDAYS_H

#include <QtCore>


class WeekDays {
private:
    bool m_monday, m_tuesday, m_wednesday, m_thursday, m_friday, m_saturday, m_sunday, m_holiday, m_school, m_noSchool;

public:
    WeekDays();
    WeekDays(int code);
    WeekDays(bool monday, bool tuesday, bool wednesday, bool thursday, bool friday, bool saturday, bool sunday, bool holiday, bool school, bool noSchool);

    static const int MonFri = 995;
    static const int Sat = 19;
    static const int Sun = 15;

    void setMonday(bool b);
    void setTuesday(bool b);
    void setWednesday(bool b);
    void setThursday(bool b);
    void setFriday(bool b);
    void setSaturday(bool b);
    void setSunday(bool b);
    void setHoliday(bool b);
    void setSchool(bool b);
    void setNoSchool(bool b);

    bool monday();
    bool tuesday();
    bool wednesday();
    bool thursday();
    bool friday();
    bool saturday();
    bool sunday();
    bool holiday();
    bool school();
    bool noSchool();

    void setCode(int code);
    int toCode();

    QString toString();

    WeekDays shfitedToNextDay();

    bool isIn(WeekDays w);
    static WeekDays combine(QList<WeekDays>);
    static bool overlap(QList<WeekDays>);
    static WeekDays intersection(QList<WeekDays>);

    static WeekDays combine(WeekDays w1, WeekDays w2);
    static bool overlap(WeekDays w1, WeekDays w2);
    static WeekDays intersection(WeekDays w1, WeekDays w2);

    bool operator ==(WeekDays *w);
    bool operator <=(WeekDays *w);

    bool operator ==(WeekDays w);
    bool operator <=(WeekDays w);

};

#endif // WEEKDAYS_H
