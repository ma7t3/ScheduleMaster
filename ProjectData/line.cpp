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
QString Line::getDescription() { return m_description; }
QColor Line::getColor()        { return m_color; }

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

/*int Line::forwardTripCount() { return m_forwardTrips.count(); }

Trip *Line::forwardTripAt(int i)
{
    if(i < 0 || i >= m_forwardTrips.count())
        return nullptr;

    return m_forwardTrips[i];
}

QList<Trip *> Line::forwardTrips() { return m_forwardTrips; }

int Line::reverseTripCount() { return m_reverseTrips.count(); }

Trip *Line::reverseTripAt(int i)
{
    if(i < 0 || i >= m_reverseTrips.count())
        return nullptr;

    return m_reverseTrips[i];
}

QList<Trip *> Line::reverseTrips() { return m_reverseTrips; }

int Line::tripCount() { return forwardTripCount() + reverseTripCount(); }

Trip *Line::TripAt(int i)
{
    QList<Trip *> list = trips();
    if(i < 0 || i >= list.count())
        return nullptr;

    return list[i];
}

QList<Trip *> Line::trips() { return forwardTrips() << reverseTrips(); }


Trip *Line::trip(QString id)
{
    for(int i = 0; i < forwardTripCount(); i++) {
        Trip *t = forwardTripAt(i);
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

    for(int i = 0; i < reverseTripCount(); i++) {
        Trip *t = reverseTripAt(i);
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

bool Line::tripDirection(Trip *t) {
    for(int i = 0; i < m_reverseTrips.count(); i++)
        if(m_reverseTrips[i] == t)
            return false;

    return true;
}

void Line::addForwardTrip(Trip *t) { m_forwardTrips << t; }
void Line::addReverseTrip(Trip *t) { m_reverseTrips << t; }

void Line::addTrip(Trip *t) {
    if(!t)
        return;

    if(t->route()->direction())
        m_forwardTrips << t;
    else
        m_reverseTrips << t;
}

void Line::removeTrip(Trip *t) {
    for(int i = 0; i < m_forwardTrips.count(); i++)
        if(m_forwardTrips[i] == t) {
            m_forwardTrips.remove(i);
            return;
        }

    for(int i = 0; i < m_reverseTrips.count(); i++)
        if(m_reverseTrips[i] == t) {
            m_reverseTrips.remove(i);
            return;
        }
}

void Line::removeTrip(QString) {
    for(int i = 0; i < m_forwardTrips.count(); i++)
        if(m_forwardTrips[i]->id() == id()) {
            m_forwardTrips.remove(i);
            return;
        }

    for(int i = 0; i < m_reverseTrips.count(); i++)
        if(m_reverseTrips[i]->id() == id()) {
            m_reverseTrips.remove(i);
            return;
        }
}

void Line::setForwardTripList(QList<Trip *> l) { m_forwardTrips = l; }
void Line::setReverseTripList(QList<Trip *> l) { m_reverseTrips = l; }*/
