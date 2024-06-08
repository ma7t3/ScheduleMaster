#include "footnote.h"

Footnote::Footnote(QObject *parent, const QString &id) :
    ProjectDataItem(parent, id),
    _autoAssignWeekDaysEnabled(false) {}

Footnote::Footnote(QObject *parent, const QJsonObject &jsonObject) : ProjectDataItem(parent) {
    fromJson(jsonObject);
}

Footnote::Footnote(const Footnote &other) :
    ProjectDataItem(other.parent()) {
    copy(other);
}

Footnote Footnote::operator=(const Footnote &other) {
    copy(other);
    return *this;
}

bool Footnote::operator<(const Footnote &other) {
    return name() < other.name();
}

void Footnote::copy(const Footnote &other) {
    ProjectDataItem::copy(other);
    setIdentifier(other.identifier());
    setName(other.name());
    setDescription(other.description());
    setAutoAssignWeekDaysEnabled(other.autoAssignWeekDaysEnabled());
    setAutoAssignWeekDays(other.autoAssignWeekDays());
    setAutoAssignCareWeekDays(other.autoAssignCareWeekDays());
}

void Footnote::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);

    setIdentifier(jsonObject.value("identifier").toString(tr("X")));
    setName(jsonObject.value("name").toString(tr("Unnamed Footnote")));
    setDescription(jsonObject.value("description").toString());

    QJsonObject jsonAutoAssignObject = jsonObject.value("autoAssign").toObject(QJsonObject());

    if(!jsonAutoAssignObject.empty() && jsonAutoAssignObject.value("weekDays").isArray()) {
        setAutoAssignWeekDaysEnabled(true);

        QJsonArray jArr = jsonAutoAssignObject.value("weekDays").toArray();
        WeekDays w, cW;

        w.setCode(jArr.at(0).toInt());
        cW.setCode(jArr.at(1).toInt());

        setAutoAssignWeekDays(w);
        setAutoAssignCareWeekDays(cW);
    } else {
        setAutoAssignWeekDaysEnabled(false);
    }
}

QJsonObject Footnote::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("identifier", identifier());
    jsonObject.insert("name", name());
    jsonObject.insert("description", description());

    QJsonObject jsonAutoAssignObject;

    if(autoAssignWeekDaysEnabled()) {
        jsonAutoAssignObject.insert("weekDays", QJsonArray() << autoAssignWeekDays().toCode() << autoAssignCareWeekDays().toCode());
    } else {
        jsonAutoAssignObject.insert("weekDays", false);
    }

    jsonObject.insert("autoAssign", jsonAutoAssignObject);
    return jsonObject;
}


WeekDays Footnote::autoAssignCareWeekDays() const {
    return _autoAssignCareWeekDays;
}

void Footnote::setAutoAssignCareWeekDays(const WeekDays &newAutoAssignCareWeekDays) {
    _autoAssignCareWeekDays = newAutoAssignCareWeekDays;
}

WeekDays Footnote::autoAssignWeekDays() const {
    return _autoAssignWeekDays;
}

void Footnote::setAutoAssignWeekDays(const WeekDays &newAutoAssignWeekDays) {
    _autoAssignWeekDays = newAutoAssignWeekDays;
}

bool Footnote::autoAssignWeekDaysEnabled() const {
    return _autoAssignWeekDaysEnabled;
}

void Footnote::setAutoAssignWeekDaysEnabled(bool newAutoAssignWeekDaysEnabled) {
    _autoAssignWeekDaysEnabled = newAutoAssignWeekDaysEnabled;
}

QString Footnote::identifier() const {
    return _identifier;
}

void Footnote::setIdentifier(const QString &newIdentifier) {
    _identifier = newIdentifier;
}

QString Footnote::name() const {
    return _name;
}

void Footnote::setName(const QString &newName) {
    _name = newName;
}

QString Footnote::description() const {
    return _description;
}

void Footnote::setDescription(const QString &newDescription) {
    _description = newDescription;
}


