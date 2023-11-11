#ifndef LINE_H
#define LINE_H

#pragma once

#include <QtCore>
#include <QColor>

#include "route.h"
#include "trip.h"
#include "linedirection.h"
#include "abstractprojectdataitem.h"

class Line : public AbstractProjectDataItem
{
private:
    QString m_name;
    QString m_description;
    QColor m_color;
    QList<LineDirection *> m_directions;
    QList<Route*> m_routes;
    /*QList<Trip *> m_forwardTrips;
    QList<Trip *> m_reverseTrips;*/
    QList<Trip *> m_trips;

public:
    Line(QString id, QString name, QString description = "", QColor color = QColor(0, 0, 0));

    void setName(QString);
    void setDescription(QString);
    void setColor(QColor);

    QString name();
    QString getDescription();
    QColor getColor();

    int directionCount();
    LineDirection *directionAt(int i);
    LineDirection *direction(QString);
    QList<LineDirection *> directions();

    void addDirection(LineDirection *);
    void removeDirection(LineDirection *);
    void removeDirection(QString);

    void addRoute(Route *);
    void removeRoute(Route *);
    void removeRoute(QString);
    void setRouteList(QList<Route *>);

    int routeCount();
    Route *route(QString);
    Route *routeAt(int);
    QList<Route *> routes();

    int tripCount();
    Trip *tripAt(int);
    Trip *trip(QString);
    QList<Trip *> trips();
    QList<Trip *> tripsToDirection(LineDirection *);
    void setTripList(QList<Trip *>);

    void addTrip(Trip *);
    void removeTrip(Trip *);
    void removeTrip(QString);

    LineDirection *directionOfTrip(Trip *);

    /*int forwardTripCount();
    Trip *forwardTripAt(int);
    QList<Trip *> forwardTrips();

    int reverseTripCount();
    Trip * reverseTripAt(int);
    QList<Trip *> reverseTrips();

    int tripCount();
    //Trip *TripAt(int);
    QList<Trip *> trips();

    Trip *trip(QString);
    bool tripDirection(Trip *t);*/

    /*void addForwardTrip(Trip *);
    void addReverseTrip(Trip *);*/

    //void addTrip(Trip *);

    /*void removeTrip(Trip *);
    void removeTrip(QString);

    void setForwardTripList(QList<Trip *>);
    void setReverseTripList(QList<Trip *>);*/
};

#endif // LINE_H
