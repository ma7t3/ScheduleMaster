#include "daytype.h"

DayType::DayType(QObject *parent, const QString &id) {
    setParent(parent);
    _id = id;
}

DayType::DayType(QObject *parent, const QString &id, const QString &name, const int &code) :
    WeekDays(code),
    _name(name) {
    setParent(parent);
    _id = id;
}

DayType::DayType(QObject *parent,
                 const QString &id,
                 const QString &name,
                 const bool &monday,
                 const bool &tuesday,
                 const bool &wednesday,
                 const bool &thursday,
                 const bool &friday,
                 const bool &saturday,
                 const bool &sunday,
                 const bool &holiday,
                 const bool &school,
                 const bool &noSchool) :
    WeekDays(monday, tuesday, wednesday, thursday, friday, saturday, sunday, holiday, school, noSchool),
    _name(name) {
    setParent(parent);
    _id = id;
}

DayType::DayType(QObject *parent, const QJsonObject &jsonObject) :
    WeekDays(jsonObject.value("code").toInt(799)) {
    setParent(parent);
    fromJson(jsonObject);
}

DayType::DayType(const DayType &other) :
    WeekDays(other) {
    setParent(other.parent());
    copy(other);
}

DayType DayType::operator=(const DayType &other) {
    copy(other);
    return *this;
}

DayType DayType::operator=(const WeekDays &other) {
    WeekDays::copy(other);
    return *this;
}

void DayType::copy(const DayType &other) {
    WeekDays::copy(other);
    setName(other.name());
}

void DayType::fromJson(const QJsonObject &jsonObject) {
    WeekDays::fromJson(jsonObject);
    setName(jsonObject.value("name").isString() ? jsonObject.value("name").toString() : tr("Unnamed DayType"));
}

QJsonObject DayType::toJson() const {
    QJsonObject jsonObject = WeekDays::toJson();
    jsonObject.insert("name", name());
    return jsonObject;
}

void DayType::setName(const QString &name) {
    _name = name;
    emit changed(this);
}

QString DayType::name() const {
    return _name;
}





