#ifndef TIMEPROFILE_H
#define TIMEPROFILE_H

#include <QtCore>

#pragma once

#include "abstractprojectdataitem.h"
#include "ProjectData/busstop.h"

class TimeProfileItem
{
private:
    float _arrValue;
    float _depValue;
    bool _seperateTimes;
    int _busstopMode;
    QString _busstopId;

public:
    TimeProfileItem(QString busstopId);
    TimeProfileItem(Busstop *busstop);

    static const int busstopModeNever = 0;
    static const int busstopModeNormal = 1;
    static const int busstopModeAlways = 2;
    static const int busstopModeWait = 3;
    static const int busstopModeAlwaysAndWait = 4;

    void setArrValue(float);
    void setDepValue(float);
    void removeArrValue();
    void setBusstopMode(int);

    QString busstopId();
    float arrValue();
    float depValue();
    bool hasSeperateTimes();
    int busstopMode();


};

class TimeProfile : public AbstractProjectDataItem
{
private:
    QString _name;
    float _duration;
    QList<TimeProfileItem*> _items;

public:
    TimeProfile(QString id, QString name);

    void setName(QString);
    void setDuration(float);
    void addBusstop(TimeProfileItem *);
    void setBusstopList(QList<TimeProfileItem *>);
    void addBusstopList(QList<TimeProfileItem *>);

    QString name();
    float duration();
    QList<TimeProfileItem *> busstops();

    TimeProfileItem *busstop(QString);
    TimeProfileItem *busstop(Busstop *);
    void removeBusstop(QString);
};

#endif // TIMEPROFILE_H
