#ifndef WEEKDAYS_H
#define WEEKDAYS_H

#include <QtCore>

#include "ProjectDataItem.h"

enum WeekDay {
    NoWeekDay = 0x000,
    monday    = 0x001,
    tuesday   = 0x002,
    wednesday = 0x004,
    thursday  = 0x008,
    friday    = 0x010,
    saturday  = 0x020,
    sunday    = 0x040,
    holiday   = 0x080,
    school    = 0x100,
    vacation  = 0x200
};

Q_DECLARE_FLAGS(WeekDaysFlag, WeekDay)

Q_DECLARE_OPERATORS_FOR_FLAGS(WeekDaysFlag)

class WeekDays : public ProjectDataItem {
    Q_OBJECT
public:
    WeekDays();
    WeekDays(const int &code);
    WeekDays(const WeekDay &w);
    WeekDays(const bool & monday, const bool & tuesday, const bool & wednesday, const bool & thursday, const bool & friday, const bool & saturday, const bool & sunday, const bool & holiday, const bool & school, const bool & vacation);
    WeekDays(const WeekDays &);

    bool operator==(const WeekDays &) const;
    bool operator!=(const WeekDays &) const;
    bool operator<=(const WeekDays &) const;
    bool operator<(const WeekDays &) const;

    WeekDays operator=(const WeekDays &);

    bool day(const WeekDaysFlag &) const;
    bool dayAtIndex(const int &) const;

    void setDay(const WeekDaysFlag &, const bool &);
    void setDayAtIndex(const int &, const bool &);

    void setCode(const int &code);
    int toCode() const;

    QString toString() const;

    WeekDays shfitedToNextDay() const;

    bool isIn(const WeekDays &w) const;
    static WeekDays combine(const QList<WeekDays> &);
    static bool overlap(const QList<WeekDays> &);
    static WeekDays intersection(const QList<WeekDays> &);

    static WeekDays combine(const WeekDays &w1, const WeekDays &w2);
    static bool overlap(const WeekDays &w1, const WeekDays &w2);
    static WeekDays intersection(const WeekDays &w1, const WeekDays &w2);

    QJsonObject toJson() const;

protected:
    void copy(const WeekDays &);
    void fromJson(QJsonObject const &);

private:
    //std::unordered_map<int, bool> _days;

    QFlags<WeekDay> _days;
};

#endif // WEEKDAYS_H
