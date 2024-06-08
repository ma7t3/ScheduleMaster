#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include "daytype.h"

class ProjectSettings : public ProjectDataItem {
    Q_OBJECT
public:
    ProjectSettings(QObject *parent = nullptr);
    ProjectSettings(const ProjectSettings &);
    ProjectSettings operator=(const ProjectSettings &);

    QString displayName() const;
    void setDisplayName(const QString &newDisplayName);

    QString shortName() const;
    void setShortName(const QString &newShortname);
    void setNames(const QString &newDisplayName, const QString &newShortName);

    QString icon() const;
    void setIcon(const QString &newIcon);


    QList<DayType *> dayTypes() const;
    int dayTypeCount() const;
    DayType *dayType(const QString &id) const;
    DayType *dayTypeAt(const int &index) const;
    bool hasDaytype(const QString &id) const;

    void setDayTypes(const QList<DayType *> &newDayTypes);
    void addDayType(DayType *);
    void removeDayType(DayType *);
    void removeDayType(const QString &id);
    void clearDayTypes();

    QJsonObject toJson() const;
    void setJson(const QJsonObject &);

    DayType *newDayType(QString id = "");
    DayType *newDayType(const QJsonObject &);
    DayType *newDayType(const DayType &newDayType);

protected:
    void copy(const ProjectSettings &);

private:
    QString _displayName;
    QString _shortName;
    QString _icon;
    QList<DayType *> _dayTypes;
};

#endif // PROJECTSETTINGS_H
