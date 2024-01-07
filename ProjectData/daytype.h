#ifndef DAYTYPE_H
#define DAYTYPE_H

#include "ProjectDataItem.h"
#include "weekdays.h"

// ABGESCHLOSSEN!

class DayType : public virtual ProjectDataItem, public WeekDays {
    Q_OBJECT
public:
    DayType(const QString &id);
    DayType(const QString &id, const QString &name, const int &code);
    DayType(const QString &id, const QString &name, const bool &monday, const bool &tuesday, const bool &wednesday, const bool &thursday, const bool &friday, const bool &saturday, const bool &sunday, const bool &holiday, const bool &school, const bool &noSchool);
    DayType(const DayType &);
    DayType operator=(const DayType &);

    void setName(const QString &);
    QString name() const;

protected:
    void copy(const DayType &);

private:
    QString _name;
};

#endif // DAYTYPE_H
