#ifndef TRIP_H
#define TRIP_H

#include <QTime>
#include "timeProfile.h"
#include "ProjectDataItem.h"
#include "busstop.h"
#include "weekdays.h"
#include "route.h"

class Trip : public ProjectDataItem {
    Q_OBJECT
public:
    Trip(QObject *parent, const QString &id);
    Trip(QObject *parent, const QJsonObject &);
    Trip(const Trip &);
    bool operator<(const Trip &);
    Trip operator=(const Trip &);

    Route *route() const;
    void setRoute(Route *);

    QTime startTime() const;
    void setStartTime(const QTime &);

    QTime endTime() const;
    QTime duration() const;
    QTime busstopTime(Busstop *) const;
    QTime busstopTime(const QString &id) const;

    TimeProfile *timeProfile() const;
    void setTimeProfile(TimeProfile *);

    WeekDays weekDays() const;
    void setWeekDays(const WeekDays &);

    bool goesPastMidnight() const;
    bool busstopIsAfterMidnight(Busstop *b) const;
    bool busstopIsAfterMidnight(const QString &id) const;

    QJsonObject toJson() const;

signals:
    void changed(Trip *);

protected:
    void copy(const Trip &);
    void fromJson(const QJsonObject &);

private:
    Route *_route;
    QTime _startTime;
    WeekDays _weekDays;
    TimeProfile *_timeProfile;
};

#endif // TRIP_H
