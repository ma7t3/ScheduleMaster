#include "publishedlinedirection.h"

PublishedLineDirection::PublishedLineDirection(const QString &id, const QString &name) :
    AbstractProjectDataItem(id),
    _name(name) {

}

QString PublishedLineDirection::name() const {
    return _name;
}

void PublishedLineDirection::setName(const QString &newName) {
    _name = newName;
}

QList<PublishedBusstop *> PublishedLineDirection::busstops() const {
    return _busstops;
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

    return _busstops[index];
}

int PublishedLineDirection::busstopCount() const {
    return _busstops.count();
}

void PublishedLineDirection::setBusstops(const QList<PublishedBusstop *> &newBusstops) {
    _busstops = newBusstops;
}

void PublishedLineDirection::addBusstop(PublishedBusstop *newBusstop) {
    if(!newBusstop)
        return;

    _busstops << newBusstop ;
}

void PublishedLineDirection::removeBusstopAt(const int &index) {
    if(index < 0 || index >= busstopCount())
        return;

    _busstops.remove(index);
}

void PublishedLineDirection::removeBusstop(const QString &id) {
    for(int i = 0; i < busstopCount(); i++) {
        if(busstopAt(i)->id() == id) {
            _busstops.remove(i);
            return;
        }
    }
}

void PublishedLineDirection::removeBusstop(PublishedBusstop *b) {
    for(int i = 0; i < busstopCount(); i++) {
        if(busstopAt(i) == b) {
            _busstops.remove(i);
            return;
        }
    }
}

QList<Route *> PublishedLineDirection::routes() const {
    return _routes;
}

Route *PublishedLineDirection::routeAt(const int &index) {
    if(index < 0 || index >= routeCount())
        return nullptr;

    return _routes[index];
}

int PublishedLineDirection::routeCount() {
    return _routes.count();
}

void PublishedLineDirection::setRoutes(const QList<Route *> &newRoutes) {
    _routes = newRoutes;
}

void PublishedLineDirection::addRoute(Route *newRoute) {
    if(!newRoute)
        return;

    _routes << newRoute;
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
