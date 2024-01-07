#ifndef TRIP_H
#define TRIP_H

#include <QTime>
#include "timeProfile.h"
#include "ProjectDataItem.h"
#include "busstop.h"
#include "weekdays.h"
#include "route.h"


class Trip : public virtual ProjectDataItem {
    Q_OBJECT
private:
    Route *_route;
    QTime _startTime;
    WeekDays *_weekDays;
    TimeProfile *_timeProfile;

public:
    Trip(QString id, Route* route, QTime startTime, TimeProfile *timeProfile, WeekDays weekDays = WeekDays());

    Route *route();
    void setRoute(Route *);

    QTime startTime();
    QTime endTime();
    QTime duration();
    QTime busstopTime(Busstop *);
    QTime busstopTime(QString id);
    void setStartTime(QTime);

    TimeProfile *timeProfile();
    void setTimeProfile(TimeProfile *);

    WeekDays *weekDays();

    bool goesPastMidnight();
    bool busstopIsAfterMidnight(Busstop *b);
    bool busstopIsAfterMidnight(QString id);

    void overwrite(Trip &other);
    bool operator<(Trip &other);
};

#endif // TRIP_H
