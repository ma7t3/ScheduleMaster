#ifndef ROUTE_H
#define ROUTE_H

#include <QtCore>

#include "busstop.h"
#include "timeProfile.h"
#include "linedirection.h"
#include "abstractprojectdataitem.h"

class Route : public AbstractProjectDataItem
{
private:
    int _code;
    LineDirection *_direction;
    QString _name;
    QList<Busstop *> _busstops;
    QList<TimeProfile *> _timeProfiles;

public:
    const bool directionForward = true;
    const bool directionReverse = false;

    Route(QString id, int code, QString name, LineDirection *direction);

    void setCode(int);
    void setDirection(LineDirection *);
    void setName(QString);

    int code();
    LineDirection *direction();
    QString name();

    void addBusstop(Busstop *);
    void insertBusstop(int, Busstop *);
    void setBusstops(QList<Busstop *>);

    int busstopCount();
    Busstop *busstopAt(int);
    Busstop *firstBusstop();
    Busstop *lastBusstop();
    bool hasBusstop(Busstop *);
    bool hasBusstop(QString);
    QList<Busstop *> busstops();

    void clearBusstopList();


    int profileCount(); // old name -> alias for timeProfileCount()
    int timeProfileCount();
    TimeProfile* timeProfile(QString);
    TimeProfile* timeProfileWithName(QString);
    TimeProfile* timeProfileAt(int);
    QList<TimeProfile *> timeProfiles();

    void addTimeProfile(TimeProfile *);
    void addTimeProfiles(QList<TimeProfile *>);
    void setTimeProfiles(QList<TimeProfile *>);

    void removeTimeProfile(TimeProfile *);
    void removeTimeProfile(QString);

    int indexOfTimeProfile(TimeProfile*);

    void overwrite(Route &other);
    bool operator<(Route &other);
};

#endif // ROUTE_H
