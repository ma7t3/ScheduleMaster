#ifndef TOUR_H
#define TOUR_H

#include <QtCore>

#include "ProjectData/line.h"
#include "abstractprojectdataitem.h"
#include "trip.h"
#include "weekdays.h"

class Tour : public AbstractProjectDataItem
{
private:
    QString m_name;
    WeekDays *m_weekDays;
    QList<Trip *> m_trips;

public:
    Tour(QString id, QString name, WeekDays weekDays = WeekDays());

    void setName(QString);
    void setWeekDays(WeekDays);

    QString name();
    WeekDays *weekDays();

    int indexOfTrip(Trip *);

    void addTrip(Trip *);
    void insertTripAt(Trip *, int);
    void insertTripAfter(Trip *, Trip *);
    void setTripList(QList<Trip *>);
    void removeTrip(Trip *);

    int tripCount();
    QList<Trip *> trips();
    Trip *trip(QString);
    Trip *tripAt(int);
    bool hasTrip(Trip *);

    bool goesPastMidnight();
    bool tripIsAfterMidnight(Trip *);
    bool tripIsAfterMidnight(QString);

    QTime startTime();
    QTime endTime();
    QTime duration();
    QTime drivingTime();
    QTime breakTime();
    //QList <Line *> usedLines();
};

#endif // TOUR_H
