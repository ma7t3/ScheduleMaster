#ifndef WEEKDAYS_H
#define WEEKDAYS_H

#include <QtCore>

#include "ProjectDataItem.h"

// ABGESCHLOSSEN!

enum class WeekDay {
    monday,
    tuesday,
    wednesday,
    thursday,
    friday,
    saturday,
    sunday,
    holiday,
    school,
    vacation
};

class WeekDays : public virtual ProjectDataItem {
    Q_OBJECT
public:
    WeekDays();
    WeekDays(const int &code);
    WeekDays(const bool & monday, const bool & tuesday, const bool & wednesday, const bool & thursday, const bool & friday, const bool & saturday, const bool & sunday, const bool & holiday, const bool & school, const bool & vacation);
    WeekDays(const WeekDays &);

    bool operator==(const WeekDays &w) const;
    bool operator==(WeekDays *w) const;

    bool operator<=(const WeekDays &w) const;
    bool operator<=(WeekDays *w) const;

    WeekDays operator=(const WeekDays &);

    static const int MonFri = 995;
    static const int Sat = 19;
    static const int Sun = 15;

    bool day(const WeekDay &) const;
    void setDay(const WeekDay &, const bool &);

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

protected:
    void copy(const WeekDays &);

private:
    std::unordered_map<WeekDay, bool> _days;
};

#endif // WEEKDAYS_H
