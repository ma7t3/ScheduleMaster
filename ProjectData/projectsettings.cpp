#include "projectsettings.h"

ProjectSettings::ProjectSettings(QObject *parent) :
    ProjectDataItem(parent) {
}

ProjectSettings::ProjectSettings(const ProjectSettings &other) :
    ProjectDataItem(other.parent()) {
    copy(other);
}

ProjectSettings ProjectSettings::operator=(const ProjectSettings &other) {
    copy(other);
    return *this;
}

void ProjectSettings::reset() {
    _displayName = "";
    _shortName = "";
    _icon = "";
    qDeleteAll(_dayTypes);
    _dayTypes.clear();
}

void ProjectSettings::copy(const ProjectSettings &other) {
    ProjectDataItem::copy(other);
    setNames(other.displayName(), other.shortName());
    setIcon(other.icon());

    QList<DayType *>  newDayTypes;
    for(int i = 0; i < other.dayTypeCount(); i++) {
        DayType *dt = other.dayTypeAt(i);
        if(hasDaytype(dt->id())) {
            DayType *current = dayType(dt->id());
            *current = *dt;
            newDayTypes << current;
        } else {
            newDayTypes << dt;
        }
    }
    setDayTypes(newDayTypes);
}

void ProjectSettings::setJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);

    setDisplayName(jsonObject.value("displayName").isString() ? jsonObject.value("displayName").toString() : "");
    setShortName(jsonObject.value("shortName").isString() ? jsonObject.value("shortName").toString() : "");
    setIcon(jsonObject.value("icon").isString() ? jsonObject.value("icon").toString() : "");

    QJsonArray jDayTypes = jsonObject.value("dayTypes").isArray() ? jsonObject.value("dayTypes").toArray() : QJsonArray();

    for(int i = 0; i < jDayTypes.count(); ++i)
        if(jDayTypes.at(i).isObject())
            addDayType(newDayType(jDayTypes.at(i).toObject()));
}

DayType *ProjectSettings::newDayType(QString id) {
    if(id.isEmpty())
        id = ProjectDataItem::getNewID();
    return new DayType(this, id);
}

DayType *ProjectSettings::newDayType(const QJsonObject &obj) {
    return new DayType(this, obj);
}

DayType *ProjectSettings::newDayType(const DayType &newDayType) {
    DayType *dt = new DayType(newDayType);
    dt->setParent(this);
    return dt;
}

QJsonObject ProjectSettings::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.remove("id");

    jsonObject.insert("displayName", displayName());
    jsonObject.insert("shortName", shortName());
    jsonObject.insert("icon", icon());

    QJsonArray jDayTypes;

    for(int i = 0; i < dayTypeCount(); ++i)
        jDayTypes.append(dayTypeAt(i)->toJson());

    jsonObject.insert("dayTypes", jDayTypes);

    return jsonObject;
}


QString ProjectSettings::displayName() const {
    return _displayName;
}

void ProjectSettings::setDisplayName(const QString &newName) {
    _displayName = newName;
}

QString ProjectSettings::shortName() const {
    return _shortName;
}

void ProjectSettings::setShortName(const QString &newName) {
    _shortName = newName;
}

void ProjectSettings::setNames(const QString &newDisplayName, const QString &newShortName) {
    _displayName = newDisplayName;
    _shortName = newShortName;
}

QString ProjectSettings::icon() const {
    return _icon;
}

void ProjectSettings::setIcon(const QString &fileName) {
    _icon = fileName;
}

QList<DayType *> ProjectSettings::dayTypes() const {
    return _dayTypes;
}

int ProjectSettings::dayTypeCount() const {
    return _dayTypes.count();
}

DayType *ProjectSettings::dayType(const QString &id) const {
    for(int i = 0; i < dayTypeCount(); i++) {
        DayType *d = dayTypeAt(i);
        if(d->id() == id)
            return d;
    }

    return nullptr;
}

DayType *ProjectSettings::dayTypeAt(const int &index) const {
    if(index < 0 || index >= dayTypeCount())
        return nullptr;

    return _dayTypes[index];
}

bool ProjectSettings::hasDaytype(const QString &id) const {
    for(int i = 0; i < dayTypeCount(); i++)
        if(dayTypeAt(i)->id() == id)
            return true;

    return false;
}

void ProjectSettings::setDayTypes(const QList<DayType *> &list) {
    _dayTypes = list;
}

void ProjectSettings::addDayType(DayType *dayType) {
    if(!dayType)
        return;

    _dayTypes << dayType;
}

void ProjectSettings::removeDayType(DayType *dayType) {
    for(int i = 0; i < dayTypeCount(); i++) {
        DayType *d = dayTypeAt(i);
        if(d == dayType) {
            _dayTypes.remove(i);
            return;
        }
    }
}

void ProjectSettings::removeDayType(const QString &id) {
    for(int i = 0; i < dayTypeCount(); i++) {
        DayType *d = dayTypeAt(i);
        if(d->id() == id) {
            _dayTypes.remove(i);
            return;
        }
    }
}

void ProjectSettings::clearDayTypes() {
    _dayTypes.clear();
}
