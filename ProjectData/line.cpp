#include "ProjectData\line.h"

Line::Line(QString id, QString name, QString description, QColor color) :
    AbstractProjectDataItem(id),
    m_name(name),
    m_description(description),
    m_color(color)
{}

void Line::setName(QString n)        { m_name = n; }
void Line::setDescription(QString d) { m_description = d; }
void Line::setColor(QColor c)        { m_color = c; }

QString Line::name()           { return m_name; }
QString Line::description() { return m_description; }
QColor Line::color()        { return m_color; }

int Line::directionCount() { return m_directions.count(); }

LineDirection *Line::directionAt(int i) {
    if(i < 0 || i >= directionCount())
        return nullptr;

    return m_directions[i];
}

LineDirection *Line::direction(QString id) {
    for(int i = 0; i < directionCount(); i++) {
        LineDirection *ld = directionAt(i);
        if(ld->id() == id)
            return ld;
    }

    return nullptr;
}

QList<LineDirection *> Line::directions() { return m_directions; }

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
    m_directions = newDirections;
}

void Line::addDirection(LineDirection *ld) {
    if(!ld)
        return;

    m_directions << ld;
}

void Line::removeDirection(LineDirection *direction) {
    for(int i = 0; i < directionCount(); i++) {
        LineDirection *ld = directionAt(i);
        if(ld == direction) {
            m_directions.remove(i);
            return;
        }
    }
}

void Line::removeDirection(QString id) {
    for(int i = 0; i < directionCount(); i++) {
        LineDirection *ld = directionAt(i);
        if(ld->id() == id) {
            m_directions.remove(i);
            return;
        }
    }
}

int Line::routeCount() { return m_routes.count(); }

Route *Line::routeAt(int i)
{
    if(i < 0 || i >= m_routes.count())
        return nullptr;
    return m_routes[i];
}

Route *Line::route(QString id)
{
    for(int i = 0; i < m_routes.count(); i++)
        if(m_routes[i]->id() == id)
            return m_routes[i];

    return nullptr;
}

QList<Route *> Line::routes() { return m_routes; }

QList<Route *> Line::routesToDirection(LineDirection *ld) {
    QList<Route *> resultList;
    for(int i = 0; i < routeCount(); i++) {
        Route *r = routeAt(i);
        if(r->direction() == ld)
            resultList << r;
    }

    return resultList;
}

void Line::addRoute(Route * r)               { m_routes << r; }

void Line::removeRoute(Route *r) {
    for(int i = 0; i < routeCount(); i++) {
        if(routeAt(i) == r) {
            m_routes.remove(i);
            return;
        }
    }
}

void Line::removeRoute(QString id) {
    for(int i = 0; i < routeCount(); i++) {
        if(routeAt(i)->id() == id) {
            m_routes.remove(i);
            return;
        }
    }
}

void Line::setRouteList(QList<Route *> list) { m_routes = list; }

int Line::tripCount() { return m_trips.count(); }

Trip *Line::tripAt(int i) {
    if(i < 0 || i >= tripCount())
        return nullptr;

    return m_trips[i];
}

Trip *Line::trip(QString id) {
    for(int i = 0; i < tripCount(); i++) {
        Trip *t = tripAt(i);
        if(!t)
            continue;

        if(t->id() == id)
            return t;

        if(t->hasRepeat()) {
            for(int j = 0; j < t->childCount(); j++) {
                if(t->childAt(j)->selfChildId() == id) {
                    return t->childAt(j);
                }
            }
        }
    }

    return nullptr;
}

QList<Trip *> Line::trips() { return m_trips; }

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

void Line::setTripList(QList<Trip *> list) { m_trips = list; }

void Line::addTrip(Trip *t) {
    m_trips << t;
}

void Line::removeTrip(Trip *t) {
    for(int i = 0; i < tripCount(); i++) {
        if(tripAt(i) == t) {
            m_trips.remove(i);
            return;
        }
    }
}

void Line::removeTrip(QString id) {
    for(int i = 0; i < tripCount(); i++) {
        if(tripAt(i)->id() == id) {
            m_trips.remove(i);
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

        setDirections(newDirections);
    }
}

bool Line::operator<(Line &other) {
    return name() < other.name();
}







