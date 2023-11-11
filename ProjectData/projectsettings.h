#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include "ProjectData/daytype.h"

#pragma once

class ProjectSettings
{
public:
    explicit ProjectSettings();
    ~ProjectSettings();

    void setDisplayName(QString);
    void setShortName(QString);
    void setNames(QString, QString);
    void setIcon(QFile);

    QString displayName();
    QString shortName();
    QFile icon();

    QList<DayType *> dayTypes();
    int dayTypeCount();
    DayType *dayType(QString);
    DayType *dayTypeAt(int);

    void setDayTypes(QList<DayType *>);
    void addDayType(DayType *);
    void removeDayType(DayType *);
    void removeDayType(QString);
    void clearDayTypes();

private:
    QString m_displayName;
    QString m_shortName;
    QString m_icon;

    QList<DayType *> m_dayTypes;
};

#endif // PROJECTSETTINGS_H
