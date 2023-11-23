#ifndef DAYTYPE_H
#define DAYTYPE_H

#include "ProjectData/abstractprojectdataitem.h"
#include "ProjectData/weekdays.h"

class DayType : public AbstractProjectDataItem, public WeekDays
{
public:
    DayType(QString id);
    DayType(QString id, QString name, int code);
    DayType(QString id, QString name, bool monday, bool tuesday, bool wednesday, bool thursday, bool friday, bool saturday, bool sunday, bool holiday, bool school, bool noSchool);

    void setName(QString);
    QString name();

    void overwrite(DayType &other);
private:
    QString m_name;
};

#endif // DAYTYPE_H
