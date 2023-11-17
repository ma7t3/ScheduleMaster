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

bool PublishedLineDirection::hasRoute(Route *r) {
    for(int i = 0; i < routeCount(); i++) {
        if(routeAt(i) == r)
            return true;
    }

    return false;
}

void PublishedLineDirection::setNew(const PublishedLineDirection &newDirection) {
    setName(newDirection.name());
    setBusstops(newDirection.busstops());
    setRoutes(newDirection.routes());
}
