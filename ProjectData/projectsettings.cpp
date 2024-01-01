#include "projectsettings.h"

ProjectSettings::ProjectSettings()
{}

ProjectSettings::~ProjectSettings()
{

}

void ProjectSettings::setDisplayName(QString name)  { _displayName = name; }
void ProjectSettings::setShortName(QString name)    { _shortName = name; }

void ProjectSettings::setNames(QString displayName, QString shortName) {
    _displayName = displayName;
    _shortName = shortName;
}

void ProjectSettings::setIcon(QString fileName) { _icon = fileName; }
QString ProjectSettings::displayName()          { return _displayName; }
QString ProjectSettings::shortName()            { return _shortName; }
QString ProjectSettings::icon()                 { return _icon; }
QList<DayType *> ProjectSettings::dayTypes()    { return _dayTypes; }

int ProjectSettings::dayTypeCount() {return _dayTypes.count(); }

DayType *ProjectSettings::dayType(QString id) {
    for(int i = 0; i < dayTypeCount(); i++) {
        DayType *d = dayTypeAt(i);
        if(d->id() == id)
            return d;
    }

    return nullptr;
}

DayType *ProjectSettings::dayTypeAt(int i) {
    if(i < 0 || i >= dayTypeCount())
        return nullptr;

    return _dayTypes[i];
}

bool ProjectSettings::hasDaytype(const QString &id) {
    for(int i = 0; i < dayTypeCount(); i++)
        if(dayTypeAt(i)->id() == id)
            return true;

    return false;
}

void ProjectSettings::setDayTypes(QList<DayType *> list) {
    _dayTypes = list;
}

void ProjectSettings::addDayType(DayType *dayType) {
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

void ProjectSettings::removeDayType(QString id) {
    for(int i = 0; i < dayTypeCount(); i++) {
        DayType *d = dayTypeAt(i);
        if(d->id() == id) {
            _dayTypes.remove(i);
            return;
        }
    }
}

void ProjectSettings::clearDayTypes() { _dayTypes.clear(); }

void ProjectSettings::overwrite(ProjectSettings &other) {
    setNames(other.displayName(), other.shortName());
    setIcon(other.icon());

    QList<DayType *>  newDayTypes;
    for(int i = 0; i < other.dayTypeCount(); i++) {
        DayType dt = *other.dayTypeAt(i);

        if(hasDaytype(dt.id())) {
            // update
            DayType *current = dayType(dt.id());

            current->overwrite(dt);
            newDayTypes << current;
        } else {
            // add
            newDayTypes << &dt;
        }
    }

    setDayTypes(newDayTypes);
}

























