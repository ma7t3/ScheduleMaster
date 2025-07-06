#include "Line.h"

#include <QJsonArray>
#include <QCollator>

Line::Line(QObject *parent, const QUuid &id, const bool &isClone) :
    ProjectDataItem(parent, id, isClone) {
}

Line::Line(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

bool Line::operator<(const Line &other) const {
    QString lineNameA = name();
    QString lineNameB = other.name();

    static thread_local QCollator collator;
    collator.setNumericMode(true);
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    return collator.compare(lineNameA, lineNameB) < 0;
}

QString Line::name() const {
    return _data.name;
}

void Line::setName(const QString &newName) {
    _data.name = newName;
    emit changed();
}

QString Line::description() const {
    return _data.description;
}

void Line::setDescription(const QString &newDescription) {
    _data.description = newDescription;
    emit changed();
}

QColor Line::color() const {
    return _data.color;
}

void Line::setColor(const QColor &newColor) {
    _data.color = newColor;
    emit changed();
}

LineCardShape Line::cardShape() const {
    return _data.cardShape;
}

void Line::setCardShape(const LineCardShape &newShape) {
    _data.cardShape = newShape;
}

LineDirection *Line::createDirection(QObject *parent) {
    return new LineDirection(parent ? parent : this);
}

LineDirection *Line::createDirection(const QJsonObject &jsonObject) {
    return new LineDirection(this, jsonObject);
}

int Line::directionCount() const {
    return _data.directions.count();
}

LineDirection *Line::direction(const QUuid &id) const {
    return _data.directions.find(id);
}

PDIList<LineDirection> Line::directions() const {
    return _data.directions;
}

int Line::indexOfDirection(LineDirection *direction) const {
    return _data.directions.indexOf(direction);
}

void Line::appendDirection(LineDirection *lineDirection) {
    _data.directions.append(lineDirection);
    connect(lineDirection, &LineDirection::changed, this, [this, lineDirection](){emit directionChanged(indexOfDirection(lineDirection), lineDirection);});
    emit directionAdded(_data.directions.count() - 1, lineDirection);
    emit changed();
}

void Line::insertDirection(const int &index, LineDirection *lineDirection) {
    if(index < 0 || index > _data.directions.count())
        return;

    _data.directions.insert(index, lineDirection);
    connect(lineDirection, &LineDirection::changed, this, [this, lineDirection](){emit directionChanged(indexOfDirection(lineDirection), lineDirection);});
    emit directionAdded(index, lineDirection);
    emit changed();
}

void Line::moveDirection(const int &from, const int &to) {
    if(from < 0 || from >= directionCount() || to < 0 || to >= directionCount())
        return;

    _data.directions.insert(to, _data.directions.takeAt(from));
    emit directionMoved(from, to);
}

void Line::moveDirection(LineDirection *direction, const int &to) {
    moveDirection(indexOfDirection(direction), to);
}

void Line::removeDirection(LineDirection *lineDirection) {
    const int index = indexOfDirection(lineDirection);
    _data.directions.remove(lineDirection);
    lineDirection->disconnect(this);
    emit directionRemoved(index, lineDirection);
    emit changed();
}

void Line::removeDirection(const QUuid &id) {
    removeDirection(direction(id));
}

Route *Line::createRoute(QObject *parent) {
    return new Route(parent ? parent : this);
}

Route *Line::createRoute(const QJsonObject &jsonObject) {
    return new Route(this, jsonObject);
}

int Line::routeCount() const {
    return _data.routes.count();
}

Route *Line::route(const QUuid &id) const {
    return _data.routes.value(id, nullptr);
}

PDISet<Route> Line::routes() const {
    return _data.routes;
}

void Line::addRoute(Route *route) {
    if(!route)
        return;

    _data.routes.add(route);
    connect(route, &Route::changed, this, [this, route](){emit routeChanged(route);});
    emit routeAdded(route);
    emit changed();
}

void Line::removeRoute(Route *route) {
    if(!route)
        return;

    route->disconnect(this);
    _data.routes.remove(route);
    emit routeRemoved(route);
    emit changed();
}

void Line::removeRoute(const QUuid &id) {
    removeRoute(route(id));
}

PDISet<Busstop> Line::allBusstops() const {
    PDISet<Busstop> set;
    for(Route *r : _data.routes)
        for(RouteBusstopItem *b : r->busstops())
            set.add(b->busstop());

    return set;
}

QJsonObject Line::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("name",        _data.name);
    jsonObject.insert("description", _data.description);
    jsonObject.insert("color",       _data.color.name());

    QJsonArray jsonDirections;
    for(LineDirection *current : _data.directions)
        jsonDirections.append(current->toJson());

    QJsonArray jsonRoutes;
    for(Route *current : _data.routes)
        jsonRoutes.append(current->toJson());

    jsonObject.insert("directions", jsonDirections);
    jsonObject.insert("routes", jsonRoutes);
    return jsonObject;
}

void Line::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
    setName(jsonObject.value("name").toString(tr("Unnamed line")));
    setDescription(jsonObject.value("description").toString());
    setColor(QColor(jsonObject.value("color").toString()));

    const QJsonArray jsonDirections = jsonObject.value("directions").toArray();
    for(const QJsonValue &val : jsonDirections)
        _data.directions.append(createDirection(val.toObject()));

    const QJsonArray jsonRoutes = jsonObject.value("routes").toArray();
    for(const QJsonValue &val : jsonRoutes) {
        Route *r = createRoute(val.toObject());
        if(route(r->id()))
            r->setID(generateID()); // NOTE: This was only for testing purposes. If you have no idea, why this is, you can safely remove it! :)))

        _data.routes.add(r);
    }
}
