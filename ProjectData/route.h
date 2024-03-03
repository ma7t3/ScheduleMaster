#ifndef ROUTE_H
#define ROUTE_H

#include <QtCore>

#include "busstop.h"
#include "timeProfile.h"
#include "linedirection.h"
#include "ProjectDataItem.h"

class Route : public virtual ProjectDataItem {
    Q_OBJECT
public:
    Route(QObject *parent, const QString &id, const int &code, const QString &name, LineDirection *direction);
    Route(QObject *parent, const QJsonObject &);
    Route(const Route &);
    bool operator<(const Route &);
    Route operator=(const Route &);

    int code() const;
    void setCode(const int &);

    QString name() const;
    void setName(const QString &);

    LineDirection *direction() const;
    void setDirection(LineDirection *);

    QList<Busstop *> busstops() const;
    int busstopCount() const;
    Busstop *busstopAt(const int &) const;
    Busstop *firstBusstop() const;
    Busstop *lastBusstop() const;
    bool hasBusstop(Busstop *) const;
    bool hasBusstop(const QString &) const;

    void setBusstops(const QList<Busstop *> &);
    void addBusstop(Busstop *);
    void insertBusstop(const int &, Busstop *);
    void clearBusstops();

    QList<TimeProfile *> timeProfiles() const;
    int timeProfileCount() const;
    TimeProfile* timeProfile(const QString &) const;
    TimeProfile* timeProfileAt(const int &) const;
    TimeProfile* timeProfileWithName(const QString &) const;
    bool hasTimeProfile(const QString &id) const;

    void setTimeProfiles(QList<TimeProfile *>);
    void addTimeProfile(TimeProfile *);
    void addTimeProfiles(QList<TimeProfile *>);
    void removeTimeProfile(TimeProfile *);
    void removeTimeProfile(const QString &);
    int indexOfTimeProfile(TimeProfile*) const;

    QJsonObject toJson() const;

protected:
    void copy(const Route &);
    void fromJson(const QJsonObject &);

private:
    int _code;
    LineDirection *_direction;
    QString _name;
    QList<Busstop *> _busstops;
    QList<TimeProfile *> _timeProfiles;
};

#endif // ROUTE_H
