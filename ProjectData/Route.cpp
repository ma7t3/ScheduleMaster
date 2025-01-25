#include "Route.h"

#include "Line.h"

Route::Route(QObject *parent, const QUuid &id) :
    ProjectDataItem(parent, id) {
}

Route::Route(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

bool Route::operator<(const Route &other) const {
    return code() < other.code();
}

Route::Data Route::data() const {
    return _data;
}

void Route::setData(const Data &newData) {
    _data = newData;
}

QString Route::name() const {
    return _data.name;
}

void Route::setName(const QString &newName) {
    _data.name = newName;
}

int Route::code() const {
    return _data.code;
}

void Route::setCode(const int &newCode) {
    _data.code = newCode;
}

LineDirection *Route::direction() const {
    LineDirection *ld = _data.direction;
    if(!ld)
        return nullptr;

    return ld->inUse() ? ld : nullptr;
}

void Route::setDirection(LineDirection *newDirection) {
    if(newDirection && newDirection->inUse())
        _data.direction = newDirection;
}

QJsonObject Route::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("name", name());
    jsonObject.insert("code", code());
    LineDirection *ld = direction();
    jsonObject.insert("direction", ld ? ld->id().toString() : "");

    return jsonObject;
}

void Route::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);
    _data.name      = jsonObject.value("name").toString(tr("unnamed route"));
    _data.code      = jsonObject.value("code").toInt(0);
    _data.direction = qobject_cast<Line *>(parent())->direction(QUuid::fromString(jsonObject.value("direction").toString()));
}

