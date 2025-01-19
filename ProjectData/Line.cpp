#include "ProjectData\line.h"

Line::Line(QObject *parent, const QString &id) :
    ProjectDataItem(parent, id),
    _hourBreak(0),
    _updateTimer(new QTimer(this)) {
    _updateTimer->setSingleShot(true);
    connect(_updateTimer, &QTimer::timeout, this, &Line::onUpdateTimerTimeout);
}

Line::Line(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent),
    _updateTimer(new QTimer(this)) {
    _updateTimer->setSingleShot(true);
    connect(_updateTimer, &QTimer::timeout, this, &Line::onUpdateTimerTimeout);
    fromJson(jsonObject);
}

Line::Line(const Line &other) :
    ProjectDataItem(other.parent()),
    _updateTimer(new QTimer(this)) {
    _updateTimer->setSingleShot(true);
    connect(_updateTimer, &QTimer::timeout, this, &Line::onUpdateTimerTimeout);
    copy(other);
}

Line Line::operator=(const Line &other) {
    copy(other);
    return *this;
}

bool Line::operator<(const Line &other) {
    bool ok1, ok2;
    int number1 = name().toInt(&ok1);
    int number2 = other.name().toInt(&ok2);

    if(ok1 && ok2)
        return number1 < number2;
    else if(!ok1 && !ok2)
        return name() < other.name();
    else if(ok1 && !ok2)
        return true;
    return false;
}

void Line::copy(const Line &other) {
    ProjectDataItem::copy(other);
    setName(other.name());
    setDescription(other.description());
    setColor(other.color());
    setHourBreak(other.hourBreak());

    QList<LineDirection *> newDirections;
    for(int i = 0; i < other.directionCount(); i++) {
        LineDirection *ld = other.directionAt(i);
        if(hasDirection(ld->id())) {
            LineDirection *currentLd = direction(ld->id());
            *currentLd = *ld;
            newDirections << currentLd;
        } else {
            newDirections << ld;
        }
    }

    setDirections(newDirections);

    QList<Route *> newRoutes;
    for(int i = 0; i < other.routeCount(); i++) {
        Route *r = other.routeAt(i);
        if(hasRoute(r->id())) {
            Route *currentRoute = route(r->id());
            *currentRoute = *r;
            newRoutes << currentRoute;
        } else {
            newRoutes << r;
        }
    }
    setRoutes(newRoutes);

    QList<Trip *> newTrips;
    for(int i = 0; i < other.tripCount(); i++) {
        Trip *t = other.tripAt(i);
        if(hasTrip(t->id())) {
            Trip *currentTrip = trip(t->id());
            *currentTrip = *t;
            newTrips << currentTrip;
        } else {
            newTrips << t;
        }
    }
    setTrips(newTrips);

    emit changed(this);
}

void Line::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);

    setName(jsonObject.value("name").toString(tr("Unnamed Line")));
    setDescription(jsonObject.value("description").toString(""));
    setColor(QColor(jsonObject.value("color").toString("#000000")));
    setHourBreak(jsonObject.value("hourBreak").toInt(0));

    QJsonArray jDirections = jsonObject.value("directions").isArray() ? jsonObject.value("directions").toArray() : QJsonArray();
    QJsonArray jRoutes = jsonObject.value("routes").isArray() ? jsonObject.value("routes").toArray() : QJsonArray();
    QJsonArray jTrips = jsonObject.value("trips").isArray() ? jsonObject.value("trips").toArray() : QJsonArray();

    for(int i = 0; i < jDirections.count(); ++i)
        if(jDirections[i].isObject())
            addDirection(newDirection(jDirections[i].toObject()));

    for(int i = 0; i < jRoutes.count(); ++i)
        if(jRoutes[i].isObject())
            addRoute(newRoute(jRoutes[i].toObject()));

    for(int i = 0; i < jTrips.count(); ++i)
        if(jTrips[i].isObject())
            addTrip(newTrip(jTrips[i].toObject()));
}

void Line::onUpdateTimerTimeout() {
    if(!_addedRoutes.isEmpty())
        emit routesAdded(_addedRoutes);

    if(!_changedRoutes.isEmpty())
        emit routesChanged(_changedRoutes);

    if(!_removedRoutes.isEmpty())
        emit routesRemoved(_removedRoutes);

    if(!_addedDirections.isEmpty())
        emit directionsAdded(_addedDirections);

    if(!_changedDirections.isEmpty())
        emit directionsChanged(_changedDirections);

    if(!_removedDirections.isEmpty())
        emit directionsRemoved(_removedDirections);

    if(!_addedTrips.isEmpty())
        emit tripsAdded(_addedTrips);

    if(!_changedTrips.isEmpty())
        emit tripsChanged(_changedTrips);

    if(!_removedTrips.isEmpty())
        emit tripsRemoved(_removedTrips);

    _addedRoutes.clear();
    _changedRoutes.clear();
    _removedRoutes.clear();

    _addedDirections.clear();
    _changedDirections.clear();
    _removedDirections.clear();

    _addedTrips.clear();
    _changedTrips.clear();
    _removedTrips.clear();
}

QJsonObject Line::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();

    jsonObject.insert("name", name());
    jsonObject.insert("description", description());
    jsonObject.insert("color", color().name(QColor::HexRgb));
    jsonObject.insert("hourBreak", hourBreak());

    QJsonArray jDirections;
    QJsonArray jRoutes;
    QJsonArray jTrips;

    for(int i = 0; i < directionCount(); ++i)
        jDirections.append(directionAt(i)->toJson());

    for(int i = 0; i < routeCount(); ++i)
        jRoutes.append(routeAt(i)->toJson());

    for(int i = 0; i < tripCount(); ++i)
        jTrips.append(tripAt(i)->toJson());

    jsonObject.insert("directions", jDirections);
    jsonObject.insert("routes", jRoutes);
    jsonObject.insert("trips", jTrips);

    return jsonObject;
}

QString Line::name() const {
    return _name;
}

void Line::setName(const QString &newName) {
    _name = newName;
    emit changed(this);
}

QString Line::description() const {
    return _description;
}

void Line::setDescription(const QString &newDescription) {
    _description = newDescription;
    emit changed(this);
}

QColor Line::color() const {
    return _color;
}

void Line::setColor(const QColor &newColor) {
    _color = newColor;
    emit changed(this);
}

int Line::hourBreak() const {
    return _hourBreak;
}

void Line::setHourBreak(const int &newHourBreak) {
    _hourBreak = newHourBreak;
    emit changed(this);
    emit hourBreakChanged(newHourBreak);
}

QList<LineDirection *> Line::directions() const {
    return _directions;
}

int Line::directionCount() const {
    return _directions.count();
}

LineDirection *Line::direction(const QString &id) const {
    for(int i = 0; i < directionCount(); i++) {
        LineDirection *ld = directionAt(i);
        if(ld->id() == id)
            return ld;
    }

    return nullptr;
}

LineDirection *Line::directionAt(const int &index) const {
    if(index < 0 || index >= directionCount())
        return nullptr;

    return _directions[index];
}


bool Line::hasDirection(const QString &id) const {
    for(int i = 0; i < directionCount(); i++)
        if(directionAt(i)->id() == id)
            return true;

    return false;
}

int Line::indexOfDirection(LineDirection *ld) const {
    for(int i = 0; i < directionCount(); i++)
        if(directionAt(i) == ld)
            return i;

    return -1;
}

int Line::indexOfDirection(const QString &id) const {
    for(int i = 0; i < directionCount(); i++)
        if(directionAt(i)->id() == id)
            return i;

    return -1;
}

void Line::setDirections(const QList<LineDirection *> &newDirections) {
    _directions = newDirections;
    emit changed(this);
}

void Line::addDirection(LineDirection *ld) {
    if(!ld)
        return;

    _directions << ld;
    onDirectionAdded(ld);
    emit changed(this);
}

void Line::insertDirection(const int &index, LineDirection *ld) {
    if(ld == nullptr || index < 0 || index > directionCount())
        return;

    _directions.insert(index, ld);
    onDirectionAdded(ld);
    emit changed(this);
}

void Line::removeDirection(LineDirection *direction) {
    for(int i = 0; i < directionCount(); i++) {
        LineDirection *ld = directionAt(i);
        if(ld == direction) {
            _directions.remove(i);
            onDirectionRemoved(ld);
            emit changed(this);
            return;
        }
    }
}

void Line::removeDirection(const QString &id) {
    for(int i = 0; i < directionCount(); i++) {
        LineDirection *ld = directionAt(i);
        if(ld->id() == id) {
            _directions.remove(i);
            onDirectionRemoved(ld);
            emit changed(this);
            return;
        }
    }
}

QList<Route *> Line::routes() const {
    return _routes;
}

int Line::routeCount() const {
    return _routes.count();
}

Route *Line::route(const QString &id) const {
    for(int i = 0; i < routeCount(); i++)
        if(routeAt(i)->id() == id)
            return _routes[i];

    return nullptr;
}

Route *Line::routeAt(const int &index) const {
    if(index < 0 || index >= _routes.count())
        return nullptr;
    return _routes[index];
}

bool Line::hasRoute(const QString &id) const {
    for (int i = 0; i < routeCount(); ++i)
        if(routeAt(i)->id() == id)
            return true;

    return false;
}

QList<Route *> Line::routesToDirection(LineDirection *ld) const {
    QList<Route *> resultList;
    for(int i = 0; i < routeCount(); i++) {
        Route *r = routeAt(i);
        if(r->direction() == ld)
            resultList << r;
    }

    return resultList;
}

void Line::setRoutes(const QList<Route *> &newRoutes) {
    _routes = newRoutes;
    emit changed(this);
}

void Line::addRoute(Route *r) {
    if(!r)
        return;

    _routes << r;
    emit changed(this);
    onRouteAdded(r);
}

void Line::removeRoute(Route *r) {
    for(int i = 0; i < routeCount(); i++) {
        if(routeAt(i) == r) {
            _routes.remove(i);
            emit changed(this);
            onRouteRemoved(r);
            return;
        }
    }
}

void Line::removeRoute(const QString &id) {
    for(int i = 0; i < routeCount(); i++) {
        if(routeAt(i)->id() == id) {
            Route *r = routeAt(i);
            _routes.remove(i);
            emit changed(this);
            onRouteRemoved(r);
            return;
        }
    }
}

QList<Trip *> Line::trips() const {
    return _trips;
}


int Line::tripCount() const {
    return _trips.count();
}


Trip *Line::trip(const QString &id) const {
    for(int i = 0; i < tripCount(); i++) {
        Trip *t = tripAt(i);
        if(!t)
            continue;

        if(t->id() == id)
            return t;
    }

    return nullptr;
}

Trip *Line::tripAt(const int &index) const {
    if(index < 0 || index >= tripCount())
        return nullptr;

    return _trips[index];
}

bool Line::hasTrip(const QString &id) const {
    for (int i = 0; i < tripCount(); ++i)
        if(tripAt(i)->id() == id)
            return true;
    return false;
}

QList<Trip *> Line::tripsToDirection(LineDirection *ld) const {
    QList<Trip *> resultList;

    for(int i = 0; i < tripCount(); i++) {
        Trip *t = tripAt(i);
        if(t->route()->direction() == ld)
            resultList << t;
    }

    return resultList;
}

QList<Trip *> Line::tripsOfRoute(Route *r) const {
    QList<Trip *> resultList;

    for(int i = 0; i < tripCount(); i++) {
        Trip *t = tripAt(i);
        if(t->route() == r)
            resultList << t;
    }

    return resultList;
}

void Line::setTrips(const QList<Trip *> &newTrips) {
    _trips = newTrips;
    emit changed(this);
}

void Line::addTrip(Trip *t) {
    if(!t)
        return;

    _trips << t;
    emit changed(this);
    onTripAdded(t);
}

void Line::removeTrip(Trip *t) {
    for(int i = 0; i < tripCount(); i++) {
        if(tripAt(i) == t) {
            _trips.remove(i);
            emit changed(this);
            onTripRemoved(t);
            return;
        }
    }
}

void Line::removeTrip(const QString &id) {
    for(int i = 0; i < tripCount(); i++) {
        if(tripAt(i)->id() == id) {
            Trip *t = tripAt(i);
            _trips.remove(i);
            emit changed(this);
            onTripRemoved(t);
            return;
        }
    }
}

LineDirection *Line::directionOfTrip(Trip *t) const {
    if(!t)
        return nullptr;

    return t->route()->direction();
}


LineDirection *Line::newDirection(QString id) {
    if(id.isEmpty())
        id = ProjectDataItem::getNewID();
    LineDirection *ld = new LineDirection(this, id);
    connect(ld, &LineDirection::changed, this, &Line::onDirectionChanged);
    return ld;
}

LineDirection *Line::newDirection(const QJsonObject &obj) {
    LineDirection *ld = new LineDirection(this, obj);
    connect(ld, &LineDirection::changed, this, &Line::onDirectionChanged);
    return ld;
}

LineDirection *Line::newDirection(const LineDirection &newDirection) {
    LineDirection *ld = new LineDirection(newDirection);
    ld->setParent(this);
    connect(ld, &LineDirection::changed, this, &Line::onDirectionChanged);
    return ld;
}

QList<LineDirection *> Line::cloneDirections() const {
    QList<LineDirection *> result;
    for(int i = 0; i < directionCount(); i++)
        result << new LineDirection(*directionAt(i));

    return result;
}

Route *Line::newRoute(QString id) {
    if(id.isEmpty())
        id = ProjectDataItem::getNewID();
    Route *r = new Route(this, id);
    connect(r, &Route::changed, this, &Line::onRouteChanged);
    return r;
}

Route *Line::newRoute(const QJsonObject &obj) {
    Route *r = new Route(this, obj);
    connect(r, &Route::changed, this, &Line::onRouteChanged);
    return r;
}

Route *Line::newRoute(const Route &newRoute) {
    Route *r = new Route(newRoute);
    r->setParent(this);
    connect(r, &Route::changed, this, &Line::onRouteChanged);
    return r;
}

Trip *Line::newTrip(QString id) {
    if(id.isEmpty())
        id = ProjectDataItem::getNewID();
    Trip *t = new Trip(this, id);
    connect(t, &Trip::changed, this, &Line::onTripChanged);
    return t;
}

Trip *Line::newTrip(const QJsonObject &obj) {
    Trip *t = new Trip(this, obj);
    connect(t, &Trip::changed, this, &Line::onTripChanged);
    return t;
}

Trip *Line::newTrip(const Trip &newTrip) {
    Trip *t = new Trip(newTrip);
    t->setParent(this);
    connect(t, &Trip::changed, this, &Line::onTripChanged);
    return t;
}

void Line::onRouteAdded(Route *r) {
    if(_addedRoutes.indexOf(r) == -1)
        _addedRoutes << r;
    _updateTimer->start(0);
}

void Line::onRouteChanged(Route *r) {
    if(_changedRoutes.indexOf(r) == -1)
        _changedRoutes << r;
    _updateTimer->start(0);
}

void Line::onRouteRemoved(Route *r) {
    if(_removedRoutes.indexOf(r) == -1)
        _removedRoutes << r;
    _updateTimer->start(0);
}

void Line::onDirectionAdded(LineDirection *ld) {
    if(_addedDirections.indexOf(ld) == -1)
        _addedDirections << ld;
    _updateTimer->start(0);
}

void Line::onDirectionChanged(LineDirection *ld) {
    if(_changedDirections.indexOf(ld) == -1)
        _changedDirections << ld;
    _updateTimer->start(0);
}

void Line::onDirectionRemoved(LineDirection *ld) {
    if(_removedDirections.indexOf(ld) == -1)
        _removedDirections << ld;
    _updateTimer->start(0);
}

void Line::onTripAdded(Trip *t) {
    if(_addedTrips.indexOf(t) == -1)
        _addedTrips << t;
    _updateTimer->start(0);
}

void Line::onTripChanged(Trip *t) {
    if(_changedTrips.indexOf(t) == -1)
        _changedTrips << t;
    _updateTimer->start(0);
}

void Line::onTripRemoved(Trip *t) {
    if(_removedTrips.indexOf(t) == -1)
        _removedTrips << t;
    _updateTimer->start(0);
}
