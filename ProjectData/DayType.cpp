#include "daytype.h"

DayType::DayType(QObject *parent, const QString &id) :
    WeekDays(parent, id) {
}

DayType::DayType(QObject *parent, const QString &id, const QString &name, const int &code) :
    WeekDays(parent, id),
    _name(name) {
    setCode(code);
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
    WeekDays(
        parent, monday, tuesday, wednesday, thursday, friday, saturday, sunday, holiday, school, noSchool),
    _name(name) {
    _id = id;
}

DayType::DayType(QObject *parent, const QJsonObject &jsonObject) :
    WeekDays(parent, jsonObject.value("code").toInt(995)) {
    fromJson(jsonObject);
}

DayType::DayType(const DayType &other) :
    WeekDays(other.parent()) {
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
}

QString DayType::name() const {
    return _name;
}





