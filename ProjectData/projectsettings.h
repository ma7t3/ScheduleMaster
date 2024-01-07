#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include "daytype.h"

// ABGESCHLOSSEN!

class ProjectSettings : public virtual ProjectDataItem {
    Q_OBJECT
public:
    explicit ProjectSettings();
    ~ProjectSettings();
    ProjectSettings(const ProjectSettings&);
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

protected:
    void copy(const ProjectSettings &);

private:
    QString _displayName;
    QString _shortName;
    QString _icon;
    QList<DayType *> _dayTypes;
};

#endif // PROJECTSETTINGS_H
