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

void PublishedLineDirection::setBusstops(const QList<PublishedBusstop *> &newBusstops) {
    m_busstops = newBusstops;
}

QList<Route *> PublishedLineDirection::routes() const {
    return m_routes;
}

void PublishedLineDirection::setRoutes(const QList<Route *> &newRoutes) {
    m_routes = newRoutes;
}

void PublishedLineDirection::setNew(const PublishedLineDirection &newDirection) {
    setName(newDirection.name());
    setBusstops(newDirection.busstops());
    setRoutes(newDirection.routes());
}
