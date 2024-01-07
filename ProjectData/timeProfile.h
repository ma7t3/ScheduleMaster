#ifndef TIMEPROFILE_H
#define TIMEPROFILE_H

#include <QtCore>

#include "ProjectDataItem.h"
#include "busstop.h"

// ABGESCHLOSSEN!

class TimeProfileItem : public virtual ProjectDataItem {
    Q_OBJECT
public:
    TimeProfileItem(const QString &busstopId);
    TimeProfileItem(Busstop *busstop);
    TimeProfileItem(const TimeProfileItem &);
    TimeProfileItem operator=(const TimeProfileItem &);

    static const int BusstopModeNever = 0;
    static const int BusstopModeNormal = 1;
    static const int BusstopModeAlways = 2;
    static const int BusstopModeWait = 3;
    static const int BusstopModeAlwaysAndWait = 4;

    float arrValue() const;
    void setArrValue(const float &);
    void removeArrValue();

    float depValue() const;
    void setDepValue(const float &);

    int busstopMode() const;
    void setBusstopMode(const int &);

    QString busstopId() const;
    bool hasSeperateTimes() const;

protected:
    void copy(const TimeProfileItem &);

private:
    float _arrValue;
    float _depValue;
    bool _seperateTimes;
    int _busstopMode;
    QString _busstopId;
};

class TimeProfile : public ProjectDataItem
{
    Q_OBJECT
public:
    TimeProfile(const QString &id, const QString &name);
    TimeProfile(const TimeProfile &);
    TimeProfile operator=(const TimeProfile &);

    QString name() const;
    void setName(const QString &);

    float duration() const;
    void setDuration(const float &);

    QList<TimeProfileItem *> busstops() const;
    TimeProfileItem *busstop(Busstop *) const;
    TimeProfileItem *busstop(const QString &) const;

    void setBusstops(const QList<TimeProfileItem *> &);
    void addBusstop(TimeProfileItem *);
    void addBusstops(const QList<TimeProfileItem *> &);
    void removeBusstop(const QString &);

protected:
    void copy(const TimeProfile &);

private:
    QString _name;
    float _duration;
    QList<TimeProfileItem*> _items;
};

#endif // TIMEPROFILE_H
