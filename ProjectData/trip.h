#ifndef TRIP_H
#define TRIP_H

#include <QTime>
#include "timeProfile.h"
#include "abstractprojectdataitem.h"
#include "busstop.h"
#include "weekdays.h"
#include "route.h"


class Trip : public AbstractProjectDataItem
{
private:
    Route *m_route;
    QTime m_startTime;
    WeekDays *m_weekDays;
    TimeProfile *m_timeProfile;

    bool m_repeat;
    QString m_selfChildId;
    QTime m_repeatInterval;
    QTime m_repeatEnd;
    QStringList m_childIDs;
    QList< Trip *> m_childTrips;


public:
    Trip(QString id, Route* route, QTime startTime, TimeProfile *timeProfile, WeekDays weekDays = WeekDays());

    void setRoute(Route *);
    void setStartTime(QTime);
    void setTimeProfile(TimeProfile *);

    void setRepeat(bool);
    void setRepeatInterval(QTime);
    void setRepeatEnd(QTime);

    Route *route();

    QTime startTime();
    QTime endTime();
    QTime duration();
    QTime busstopTime(Busstop *);
    QTime busstopTime(QString id);

    WeekDays *weekDays();
    TimeProfile *timeProfile();

    bool goesPastMidnight();
    bool busstopIsAfterMidnight(Busstop *b);
    bool busstopIsAfterMidnight(QString id);

    bool hasRepeat();
    QTime repeatInterval();
    QTime repeatEnd();

    int childCount();
    int repeatTimes();

    void refreshChilds();

    Trip * childAt(int i);
    Trip * repetitionAt(int i);

    void setChildIdList(QStringList);

    QStringList childIDs();
    QString childIdAt(int);

    void setSelfChildId(QString);
    QString selfChildId();

    void overwrite(Trip &other);
    bool operator<(Trip &other);
};

#endif // TRIP_H
