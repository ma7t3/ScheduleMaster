#include "publishedlinedirection.h"

PublishedLineDirection::PublishedLineDirection(const QString &id, const QString &name) :
    AbstractProjectDataItem(id),
    m_name(name) {

}

QString PublishedLineDirection::name() const {
    return m_name;
}

void PublishedLineDirection::setName(const QString &newName) {
    m_name = newName;
}

QList<PublishedBusstop *> PublishedLineDirection::busstops() const {
    return m_busstops;
}

PublishedBusstop *PublishedLineDirection::busstop(const QString &id) {
    for(int i = 0; i < busstopCount(); i++) {
        PublishedBusstop *b = busstopAt(i);
        if(b->id() == id || b->linkedBusstop()->id() == id)
            return b;
    }

    return nullptr;
}

PublishedBusstop *PublishedLineDirection::busstopAt(const int &index) {
    if(index < 0 || index >= busstopCount())
        return nullptr;

    return m_busstops[index];
}

int PublishedLineDirection::busstopCount() const {
    return m_busstops.count();
}

void PublishedLineDirection::setBusstops(const QList<PublishedBusstop *> &newBusstops) {
    m_busstops = newBusstops;
}

void PublishedLineDirection::addBusstop(PublishedBusstop *newBusstop) {
    if(!newBusstop)
        return;

    m_busstops << newBusstop ;
}

void PublishedLineDirection::removeBusstopAt(const int &index) {
    if(index < 0 || index >= busstopCount())
        return;

    m_busstops.remove(index);
}

void PublishedLineDirection::removeBusstop(const QString &id) {
    for(int i = 0; i < busstopCount(); i++) {
        if(busstopAt(i)->id() == id) {
            m_busstops.remove(i);
            return;
        }
    }
}

void PublishedLineDirection::removeBusstop(PublishedBusstop *b) {
    for(int i = 0; i < busstopCount(); i++) {
        if(busstopAt(i) == b) {
            m_busstops.remove(i);
            return;
        }
    }
}

QList<Route *> PublishedLineDirection::routes() const {
    return m_routes;
}

Route *PublishedLineDirection::routeAt(const int &index) {
    if(index < 0 || index >= routeCount())
        return nullptr;

    return m_routes[index];
}

int PublishedLineDirection::routeCount() {
    return m_routes.count();
}

void PublishedLineDirection::setRoutes(const QList<Route *> &newRoutes) {
    m_routes = newRoutes;
}

void PublishedLineDirection::addRoute(Route *newRoute) {
    if(!newRoute)
        return;

    m_routes << newRoute;
}

bool PublishedLineDirection::hasRoute(Route *r) {
    for(int i = 0; i < routeCount(); i++) {
        if(routeAt(i) == r)
            return true;
    }

    return false;
}

void PublishedLineDirection::ovoverwrite(const PublishedLineDirection &other) {
    setName(other.name());
    setBusstops(other.busstops());
    setRoutes(other.routes());
}
