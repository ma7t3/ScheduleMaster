#ifndef TIMEPROFILE_H
#define TIMEPROFILE_H

#include <QtCore>

#pragma once

#include "abstractprojectdataitem.h"
#include "ProjectData/busstop.h"

class TimeProfileItem
{
private:
    float m_arrValue;
    float m_depValue;
    bool m_seperateTimes;
    int m_busstopMode;
    QString m_busstopId;

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
    QString m_name;
    float m_duration;
    QList<TimeProfileItem*> m_items;

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
