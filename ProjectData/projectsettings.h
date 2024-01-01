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
    void setIcon(QString);

    QString displayName();
    QString shortName();
    QString icon();

    QList<DayType *> dayTypes();
    int dayTypeCount();
    DayType *dayType(QString);
    DayType *dayTypeAt(int);
    bool hasDaytype(const QString &id);

    void setDayTypes(QList<DayType *>);
    void addDayType(DayType *);
    void removeDayType(DayType *);
    void removeDayType(QString);
    void clearDayTypes();

    void overwrite(ProjectSettings &other);

private:
    QString _displayName;
    QString _shortName;
    QString _icon;

    QList<DayType *> _dayTypes;
};

#endif // PROJECTSETTINGS_H
