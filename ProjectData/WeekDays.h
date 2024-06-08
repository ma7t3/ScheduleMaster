#ifndef WEEKDAYS_H
#define WEEKDAYS_H

#include <QtCore>

#include "ProjectDataItem.h"

enum WeekDay {
    monday    = 0,
    tuesday   = 1,
    wednesday = 2,
    thursday  = 3,
    friday    = 4,
    saturday  = 5,
    sunday    = 6,
    holiday   = 7,
    school    = 8,
    vacation  = 9
};

class WeekDays : public ProjectDataItem {
    Q_OBJECT
public:
    WeekDays(QObject *parent = nullptr);
    WeekDays(QObject *parent, const int &code);
    WeekDays(QObject *parent, const QString &id);
    WeekDays(QObject *parent, const bool & monday, const bool & tuesday, const bool & wednesday, const bool & thursday, const bool & friday, const bool & saturday, const bool & sunday, const bool & holiday, const bool & school, const bool & vacation);
    WeekDays(const WeekDays &);

    bool operator==(const WeekDays &) const;
    bool operator!=(const WeekDays &) const;
    bool operator<=(const WeekDays &) const;
    bool operator<(const WeekDays &) const;

    WeekDays operator=(const WeekDays &);

    bool day(const WeekDay &) const;
    void setDay(const WeekDay &, const bool &);

    bool day(const int &) const;
    void setDay(const int &, const bool &);

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
    std::unordered_map<int, bool> _days;
};

#endif // WEEKDAYS_H
