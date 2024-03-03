#ifndef DAYTYPE_H
#define DAYTYPE_H

#include "ProjectDataItem.h"
#include "weekdays.h"

class DayType : public virtual ProjectDataItem, public WeekDays {
    Q_OBJECT
public:
    DayType(QObject *parent, const QString &id);
    DayType(QObject *parent, const QString &id, const QString &name, const int &code);
    DayType(QObject *parent, const QString &id, const QString &name, const bool &monday, const bool &tuesday, const bool &wednesday, const bool &thursday, const bool &friday, const bool &saturday, const bool &sunday, const bool &holiday, const bool &school, const bool &noSchool);
    DayType(QObject *parent, const QJsonObject &);
    DayType(const DayType &);
    DayType operator=(const DayType &);
    DayType operator=(const WeekDays &);

    void setName(const QString &);
    QString name() const;

    QJsonObject toJson() const;

protected:
    void copy(const DayType &);
    void fromJson(const QJsonObject &);

private:
    QString _name;
};

#endif // DAYTYPE_H
