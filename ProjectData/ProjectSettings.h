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

    void setDayTypes(const QList<DayType *> &newDayTypes, const bool &doSignalConnect = false);
    void addDayType(DayType *, const bool &doSignalConnect = false);
    void insertDayType(DayType *, const int &index, const bool &doSignalConnect = false);
    void removeDayType(DayType *);
    void removeDayType(const QString &id);
    void clearDayTypes();

    QJsonObject toJson() const;
    void setJson(const QJsonObject &);

    QList<DayType *> cloneDayTypes() const;

    DayType *newDayType(QString id = "");
    DayType *newDayType(const QJsonObject &);
    DayType *newDayType(const DayType &newDayType);

protected:
    void copy(const ProjectSettings &);

    void onDayTypeAdded(DayType *);
    void onDayTypeChanged(DayType *);
    void onDayTypeRemoved(DayType *);

protected slots:
    void onUpdateTimerTimeout();

signals:
    void changed(ProjectSettings *);
    void dayTypesAdded(const QList<DayType *>);
    void dayTypesChanged(const QList<DayType *>);
    void dayTypesRemoved(const QList<DayType *>);

private:
    QString _displayName;
    QString _shortName;
    QString _icon;
    QList<DayType *> _dayTypes;

    QList<DayType *> _addedDayTypes;
    QList<DayType *> _changedDayTypes;
    QList<DayType *> _removedDayTypes;

    QTimer *_updateTimer;
};

#endif // PROJECTSETTINGS_H
