#include "projectsettings.h"

ProjectSettings::ProjectSettings()
{}

ProjectSettings::~ProjectSettings()
{

}

void ProjectSettings::setDisplayName(QString name)  { m_displayName = name; }
void ProjectSettings::setShortName(QString name)    { m_displayName = name; }

void ProjectSettings::setNames(QString displayName, QString shortName) {
    m_displayName = displayName;
    m_shortName = shortName;
}

void ProjectSettings::setIcon(QFile file)    { m_icon = file.fileName(); }
QString ProjectSettings::displayName()       { return m_displayName; }
QString ProjectSettings::shortName()         { return m_shortName; }
QFile ProjectSettings::icon()                { return QFile(m_icon); }
QList<DayType *> ProjectSettings::dayTypes() { return m_dayTypes; }

int ProjectSettings::dayTypeCount() {return m_dayTypes.count(); }

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

    return m_dayTypes[i];
}

void ProjectSettings::setDayTypes(QList<DayType *> list) {
    m_dayTypes = list;
}

void ProjectSettings::addDayType(DayType *dayType) {
    m_dayTypes << dayType;
}

void ProjectSettings::removeDayType(DayType *dayType) {
    for(int i = 0; i < dayTypeCount(); i++) {
        DayType *d = dayTypeAt(i);
        if(d == dayType) {
            m_dayTypes.remove(i);
            return;
        }
    }
}

void ProjectSettings::removeDayType(QString id) {
    for(int i = 0; i < dayTypeCount(); i++) {
        DayType *d = dayTypeAt(i);
        if(d->id() == id) {
            m_dayTypes.remove(i);
            return;
        }
    }
}

void ProjectSettings::clearDayTypes() { m_dayTypes.clear(); }




