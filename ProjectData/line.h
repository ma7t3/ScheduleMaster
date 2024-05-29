#ifndef LINE_H
#define LINE_H

#include <QtCore>
#include <QColor>

#include "route.h"
#include "trip.h"
#include "linedirection.h"
#include "ProjectDataItem.h"

#include <QObject>

class Line : public ProjectDataItem {
    Q_OBJECT
public:
    Line(QObject *parent, const QString &id);
    Line(QObject *parent, const QJsonObject &);
    Line(const Line &);

    Line operator=(const Line &);
    bool operator<(const Line &);

    QString name() const;
    void setName(const QString &);

    QString description() const;
    void setDescription(const QString &);

    QColor color() const;
    void setColor(const QColor &);


    int hourBreak() const;
    void setHourBreak(const int &newHourBreak);

    QList<LineDirection *> directions() const;
    int directionCount() const;
    LineDirection *direction(const QString &id) const;
    LineDirection *directionAt(const int &index) const;
    bool hasDirection(const QString &id) const;
    int indexOfDirection(const QString &id) const;

    void setDirections(const QList<LineDirection *> &newDirections);
    void addDirection(LineDirection *);
    void removeDirection(LineDirection *);
    void removeDirection(const QString &id);

    QList<Route *> routes() const;
    int routeCount() const;
    Route *route(const QString &id) const;
    Route *routeAt(const int &index) const;
    bool hasRoute(const QString &id) const;
    QList<Route *> routesToDirection(LineDirection *) const;

    void setRoutes(const QList<Route *> &newRoutes);
    void addRoute(Route *);
    void removeRoute(Route *);
    void removeRoute(const QString &id);

    QList<Trip *> trips() const;
    int tripCount() const;
    Trip *trip(const QString &id) const;
    Trip *tripAt(const int &index) const;
    bool hasTrip(const QString &id) const;
    QList<Trip *> tripsToDirection(LineDirection *) const;
    QList<Trip *> tripsOfRoute(Route *) const;

    void setTrips(const QList<Trip *> &newTrips);
    void addTrip(Trip *);
    void removeTrip(Trip *);
    void removeTrip(const QString &id);
    LineDirection *directionOfTrip(Trip *) const;

    QJsonObject toJson() const;

    LineDirection *newDirection(QString id = "");
    LineDirection *newDirection(const QJsonObject &obj);
    LineDirection *newDirection(const LineDirection &newDirection);

    QList<LineDirection *> cloneDirections() const;

    Route *newRoute(QString id = "");
    Route *newRoute(const QJsonObject &obj);
    Route *newRoute(const Route &newRoute);

    Trip *newTrip(QString id = "");
    Trip *newTrip(const QJsonObject &obj);
    Trip *newTrip(const Trip &newTrip);

protected:
    void copy(const Line &);
    void fromJson(const QJsonObject &);

private:
    QString _name;
    QString _description;
    QColor _color;
    QList<LineDirection *> _directions;
    QList<Route*> _routes;
    QList<Trip *> _trips;
    int _hourBreak;
};

#endif // LINE_H
