#ifndef FOOTNOTE_H
#define FOOTNOTE_H

#include <QObject>

#include "ProjectDataItem.h"

#include "weekdays.h"

class Footnote : public virtual ProjectDataItem {
    Q_OBJECT
public:
    Footnote(QObject *parent, const QString &id, const QString &identifier, const QString &name, const QString &description = "");
    Footnote(QObject *parent, const QJsonObject &);
    Footnote(const Footnote &);
    Footnote operator=(const Footnote &);
    bool operator<(const Footnote &);

    QString identifier() const;
    void setIdentifier(const QString &newIdentifier);

    QString name() const;
    void setName(const QString &newName);

    QString description() const;
    void setDescription(const QString &newDescription);

    bool autoAssignWeekDaysEnabled() const;
    void setAutoAssignWeekDaysEnabled(bool newAutoAssignWeekDaysEnabled);

    WeekDays autoAssignWeekDays() const;
    void setAutoAssignWeekDays(const WeekDays &newAutoAssignWeekDays);

    WeekDays autoAssignCareWeekDays() const;
    void setAutoAssignCareWeekDays(const WeekDays &newAutoAssignCareWeekDays);

    QJsonObject toJson() const;

protected:
    void copy(const Footnote &);
    void fromJson(const QJsonObject &);

private:
    QString _identifier, _name, _description;

    bool _autoAssignWeekDaysEnabled;
    WeekDays _autoAssignWeekDays, _autoAssignCareWeekDays;
};

#endif // FOOTNOTE_H
