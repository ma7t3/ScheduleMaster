#ifndef WEEKDAYS_H
#define WEEKDAYS_H

#include <QtCore>

#include "ProjectDataItem.h"

enum WeekDay {
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
    WeekDays(QObject *parent);
    WeekDays(QObject *parent, const int &code);
    WeekDays(QObject *parent, const bool & monday, const bool & tuesday, const bool & wednesday, const bool & thursday, const bool & friday, const bool & saturday, const bool & sunday, const bool & holiday, const bool & school, const bool & vacation);
    WeekDays(const WeekDays &);

    bool operator==(const WeekDays &w) const;
    bool operator==(WeekDays *w) const;

    bool operator<=(const WeekDays &w) const;
    bool operator<=(WeekDays *w) const;

    WeekDays operator=(const WeekDays &);

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

    QJsonObject toJson() const;

protected:
    void copy(const WeekDays &);
    void fromJson(QJsonObject const &);

private:
    std::unordered_map<WeekDay, bool> _days;
};

#endif // WEEKDAYS_H
