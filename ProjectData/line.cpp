#include "ProjectData\line.h"

Line::Line(QString id, QString name, QString description, QColor color) :
    AbstractProjectDataItem(id),
    _name(name),
    _description(description),
    _color(color)
{}

void Line::setName(QString n)        { _name = n; }
void Line::setDescription(QString d) { _description = d; }
void Line::setColor(QColor c)        { _color = c; }

QString Line::name()           { return _name; }
QString Line::description() { return _description; }
QColor Line::color()        { return _color; }

int Line::directionCount() { return _directions.count(); }

LineDirection *Line::directionAt(int i) {
    if(i < 0 || i >= directionCount())
        return nullptr;

    return _directions[i];
}

LineDirection *Line::direction(QString id) {
    for(int i = 0; i < directionCount(); i++) {
        LineDirection *ld = directionAt(i);
        if(ld->id() == id)
            return ld;
    }

    return nullptr;
}

QList<LineDirection *> Line::directions() { return _directions; }

bool Line::hasDirection(const QString &id) {
    for(int i = 0; i < directionCount(); i++)
        if(directionAt(i)->id() == id)
            return true;

    return false;
}

int Line::indexOfDirection(const QString &id) {
    for(int i = 0; i < directionCount(); i++)
        if(directionAt(i)->id() == id)
            return i;

    return -1;
}

void Line::setDirections(const QList<LineDirection *> &newDirections) {
    _directions = newDirections;
}

void Line::addDirection(LineDirection *ld) {
    if(!ld)
        return;

    _directions << ld;
}

void Line::removeDirection(LineDirection *direction) {
    for(int i = 0; i < directionCount(); i++) {
        LineDirection *ld = directionAt(i);
        if(ld == direction) {
            _directions.remove(i);
            return;
        }
    }
}

void Line::removeDirection(QString id) {
    for(int i = 0; i < directionCount(); i++) {
        LineDirection *ld = directionAt(i);
        if(ld->id() == id) {
            _directions.remove(i);
            return;
        }
    }
}

int Line::routeCount() { return _routes.count(); }

Route *Line::routeAt(int i)
{
    if(i < 0 || i >= _routes.count())
        return nullptr;
    return _routes[i];
}

Route *Line::route(QString id)
{
    for(int i = 0; i < _routes.count(); i++)
        if(_routes[i]->id() == id)
            return _routes[i];

    return nullptr;
}

QList<Route *> Line::routes() { return _routes; }

QList<Route *> Line::routesToDirection(LineDirection *ld) {
    QList<Route *> resultList;
    for(int i = 0; i < routeCount(); i++) {
        Route *r = routeAt(i);
        if(r->direction() == ld)
            resultList << r;
    }

    return resultList;
}

void Line::addRoute(Route * r)               { _routes << r; }

void Line::removeRoute(Route *r) {
    for(int i = 0; i < routeCount(); i++) {
        if(routeAt(i) == r) {
            _routes.remove(i);
            return;
        }
    }
}

void Line::removeRoute(QString id) {
    for(int i = 0; i < routeCount(); i++) {
        if(routeAt(i)->id() == id) {
            _routes.remove(i);
            return;
        }
    }
}

void Line::setRouteList(QList<Route *> list) { _routes = list; }

int Line::tripCount() { return _trips.count(); }

Trip *Line::tripAt(int i) {
    if(i < 0 || i >= tripCount())
        return nullptr;
    
    return _trips[i];
}

Trip *Line::trip(QString id) {
    for(int i = 0; i < tripCount(); i++) {
        Trip *t = tripAt(i);
        if(!t)
            continue;

        if(t->id() == id)
            return t;
    }

    return nullptr;
}

QList<Trip *> Line::trips() { return _trips; }

QList<Trip *> Line::tripsToDirection(LineDirection *ld) {
    QList<Trip *> resultList;

    for(int i = 0; i < tripCount(); i++) {
        Trip *t = tripAt(i);
        if(t->route()->direction() == ld)
            resultList << t;
    }

    return resultList;
}

QList<Trip *> Line::tripsOfRoute(Route *route) {
    QList<Trip *> resultList;

    for(int i = 0; i < tripCount(); i++) {
        Trip *t = tripAt(i);
        if(t->route() == route)
            resultList << t;
    }

    return resultList;
}

void Line::setTripList(QList<Trip *> list) { _trips = list; }

void Line::addTrip(Trip *t) {
    _trips << t;
}

void Line::removeTrip(Trip *t) {
    for(int i = 0; i < tripCount(); i++) {
        if(tripAt(i) == t) {
            _trips.remove(i);
            return;
        }
    }
}

void Line::removeTrip(QString id) {
    for(int i = 0; i < tripCount(); i++) {
        if(tripAt(i)->id() == id) {
            _trips.remove(i);
            return;
        }
    }
}

LineDirection *Line::directionOfTrip(Trip *t) {
    if(!t)
        return nullptr;

    return t->route()->direction();
}

void Line::overwrite(Line &other) {
    setName(other.name());
    setDescription(other.description());
    setColor(other.color());

    QList<LineDirection *> newDirections;
    for(int i = 0; i < other.directionCount(); i++) {
        LineDirection *ld = other.directionAt(i);

        if(hasDirection(ld->id())) {
            LineDirection *currentLd = direction(ld->id());
            currentLd->overwrite(*ld); // update
            newDirections << currentLd;
        } else {
            newDirections << ld; // add
        }
    }
    setDirections(newDirections);
}

bool Line::operator<(Line &other) {
    return name() < other.name();
}







