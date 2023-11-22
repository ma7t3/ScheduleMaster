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
    QList<Trip *> m_trips;

public:
    Line(QString id, QString name, QString description = "", QColor color = QColor(0, 0, 0));

    void setName(QString);
    void setDescription(QString);
    void setColor(QColor);

    QString name();
    QString description();
    QColor color();

    int directionCount();
    LineDirection *directionAt(int i);
    LineDirection *direction(QString);
    QList<LineDirection *> directions();
    bool hasDirection(const QString &id);
    int indexOfDirection(const QString &id);

    void setDirections(const QList<LineDirection *> &newDirections);
    void addDirection(LineDirection *);
    void removeDirection(LineDirection *);
    void removeDirection(QString);

    int routeCount();
    Route *route(QString);
    Route *routeAt(int);
    QList<Route *> routes();
    QList<Route *> routesToDirection(LineDirection *);

    void addRoute(Route *);
    void removeRoute(Route *);
    void removeRoute(QString);
    void setRouteList(QList<Route *>);    

    int tripCount();
    Trip *tripAt(int);
    Trip *trip(QString);
    QList<Trip *> trips();
    QList<Trip *> tripsToDirection(LineDirection *);
    QList<Trip *> tripsOfRoute(Route *);


    void setTripList(QList<Trip *>);
    void addTrip(Trip *);
    void removeTrip(Trip *);
    void removeTrip(QString);

    LineDirection *directionOfTrip(Trip *);

    void operator=(Line &other);
    bool operator<(Line &other);
};

#endif // LINE_H
