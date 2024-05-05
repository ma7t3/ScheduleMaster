#include "publishedlinedirection.h"

#include "ProjectData.h"

PublishedLineDirection::PublishedLineDirection(QObject *parent, const QString &id) :
    ProjectDataItem(parent, id) {}

PublishedLineDirection::PublishedLineDirection(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

PublishedLineDirection::PublishedLineDirection(const PublishedLineDirection &other) :
    ProjectDataItem(other.parent()){
    copy(other);
}

PublishedLineDirection PublishedLineDirection::operator=(const PublishedLineDirection &other) {
    copy(other);
    return *this;
}

void PublishedLineDirection::copy(const PublishedLineDirection &other) {
    ProjectDataItem::copy(other);
    setName(other.name());
    setRoutes(other.routes());

    QList<PublishedBusstop *> newBusstops;
    for(int i = 0; i < other.busstopCount(); i++) {
        PublishedBusstop *ld = other.busstopAt(i);
        if(hasBusstop(ld->id())) {
            PublishedBusstop *currentLd = busstop(ld->id());
            *currentLd = *ld;
            newBusstops << currentLd;
        } else {
            newBusstops << ld;
        }
    }
    setBusstops(newBusstops);
}

void PublishedLineDirection::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
    setName(jsonObject.value("name").toString(tr("Unnamed Direction")));

    QJsonArray jBusstops = jsonObject.value("busstops").toArray();
    QJsonArray jRoutes = jsonObject.value("routes").toArray();

    for(int i = 0; i < jBusstops.count(); ++i)
        addBusstop(newBusstop(jBusstops.at(i).toObject()));

    for(int i = 0; i < jRoutes.count(); ++i) {
        Route *r = static_cast<ProjectData *>(parent()->parent()->parent())->route(jRoutes.at(i).toString(""));
        if(r)
            addRoute(r);
    }
}


QJsonObject PublishedLineDirection::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.remove("id");
    jsonObject.insert("name", name());

    QJsonArray jBusstops, jRoutes;

    for (int i = 0; i < busstopCount(); ++i)
        jBusstops.append(busstopAt(i)->toJson());

    for (int i = 0; i < routeCount(); ++i)
        jRoutes.append(routeAt(i)->id());

    jsonObject.insert("busstops", jBusstops);
    jsonObject.insert("routes", jRoutes);

    return jsonObject;
}

void PublishedLineDirection::refreshChilds() {
    foreach (PublishedBusstop *b, _busstops)
        b->setParent(this);
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

int PublishedLineDirection::busstopCount() const {
    return _busstops.count();
}

PublishedBusstop *PublishedLineDirection::busstop(const QString &id) const {
    for(int i = 0; i < busstopCount(); i++) {
        PublishedBusstop *b = busstopAt(i);
        if(b->id() == id || b->linkedBusstop()->id() == id)
            return b;
    }

    return nullptr;
}

PublishedBusstop *PublishedLineDirection::busstopAt(const int &index) const {
    if(index < 0 || index >= busstopCount())
        return nullptr;

    return _busstops[index];
}

bool PublishedLineDirection::hasBusstop(const QString &id) const {
    for (int i = 0; i < busstopCount(); ++i)
        if(busstopAt(i)->id() == id)
            return true;
    return false;
}

void PublishedLineDirection::setBusstops(const QList<PublishedBusstop *> &newBusstops) {
    _busstops = newBusstops;
}

void PublishedLineDirection::addBusstop(PublishedBusstop *newBusstop) {
    if(!newBusstop)
        return;

    _busstops << newBusstop ;
}

void PublishedLineDirection::addBusstopAt(const int &index, PublishedBusstop *newBusstop) {
    if(!newBusstop)
        return;

    if(index < 0 || index >= busstopCount())
        _busstops << newBusstop;
    else
        _busstops.insert(index, newBusstop);
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

Route *PublishedLineDirection::routeAt(const int &index) const {
    if(index < 0 || index >= routeCount())
        return nullptr;

    return _routes[index];
}

int PublishedLineDirection::routeCount() const {
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

PublishedBusstop *PublishedLineDirection::newBusstop(QString id) {
    if(id.isEmpty())
        id = ProjectDataItem::getNewID();
    return new PublishedBusstop(this, id);
}

PublishedBusstop *PublishedLineDirection::newBusstop(const QJsonObject &obj) {
    return new PublishedBusstop(this, obj);
}

PublishedBusstop *PublishedLineDirection::newBusstop(const PublishedBusstop &newBusstop) {
    PublishedBusstop *b = new PublishedBusstop(newBusstop);
    b->setParent(this);
    return b;
}
