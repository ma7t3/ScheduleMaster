#ifndef TOUR_H
#define TOUR_H

#include <QtCore>

#include "ProjectDataItem.h"
#include "trip.h"
#include "weekdays.h"

class Tour : public virtual ProjectDataItem {
    Q_OBJECT
public:
    Tour(QObject *parent, const QString &id, const QString &name, const WeekDays &weekDays = WeekDays(nullptr));
    Tour(QObject *parent, const QJsonObject &);
    Tour(const Tour &);
    Tour operator=(const Tour &);
    bool operator<(Tour &other);

    QString name() const;
    void setName(const QString &);

    void setWeekDays(const WeekDays &);
    WeekDays *weekDays() const;


    QList<Trip *> trips() const;
    int tripCount() const;
    Trip *trip(const QString &id) const;
    Trip *tripAt(const int &index) const;
    bool hasTrip(Trip *) const;
    int indexOfTrip(Trip *) const;

    void setTrips(const QList<Trip *> &);
    void addTrip(Trip *);
    void insertTripAt(Trip *, const int &index);
    void insertTripAfter(Trip *, Trip *);
    void removeTrip(Trip *);

    bool goesPastMidnight() const;
    bool tripIsAfterMidnight(Trip *) const;
    bool tripIsAfterMidnight(const QString &id) const;

    QTime startTime() const;
    QTime endTime() const;
    QTime duration() const;
    QTime drivingTime() const;
    QTime breakTime() const;

    QJsonObject toJson() const;

protected:
    void copy(const Tour &);
    void fromJson(const QJsonObject &);

private:
    QString _name;
    WeekDays *_weekDays;
    QList<Trip *> _trips;

};

#endif // TOUR_H
