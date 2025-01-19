#ifndef ROUTE_H
#define ROUTE_H

#include <QtCore>

#include "busstop.h"
#include "timeProfile.h"
#include "linedirection.h"
#include "ProjectDataItem.h"

class Route : public ProjectDataItem {
    Q_OBJECT
public:
    Route(QObject *parent, const QString &id);
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
    Busstop *firstCommonBusstop(Route *) const;
    int indexOfBusstop(Busstop *) const;

    void setBusstops(const QList<Busstop *> &);
    void addBusstop(Busstop *);
    void insertBusstop(const int &, Busstop *);
    void removeBusstop(const int &);
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

    QList<TimeProfile *> cloneTimeProfiles() const;

    TimeProfile *newTimeProfile(QString id = "");
    TimeProfile *newTimeProfile(const QJsonObject &);
    TimeProfile *newTimeProfile(const TimeProfile &newTimeProfile);

signals:
    void changed(Route *);
    void busstopListReset();

    void busstopsAdded(QList<Busstop*>);
    void busstopsChanged(QList<Busstop*>);
    void busstopsRemoved(QList<Busstop*>);

    void timeProfilesAdded(QList<TimeProfile *>);
    void timeProfilesChanged(QList<TimeProfile *>);
    void timeProfilesRemoved(QList<TimeProfile *>);

protected:
    void copy(const Route &);
    void fromJson(const QJsonObject &);

    void onBusstopAdded(Busstop *);
    void onBusstopChanged(Busstop *);
    void onBusstopRemoved(Busstop *);

    void onTimeProfileAdded(TimeProfile *);
    void onTimeProfileChanged(TimeProfile *);
    void onTimeProfileRemoved(TimeProfile *);

protected slots:
    void onUpdateTimerTimeout();

private:
    int _code;
    LineDirection *_direction;
    QString _name;
    QList<Busstop *> _busstops;
    QList<TimeProfile *> _timeProfiles;

    QList<Busstop *> _addedBusstops;
    QList<Busstop *> _changedBusstops;
    QList<Busstop *> _removedBusstops;

    QList<TimeProfile *> _addedTimeProfiles;
    QList<TimeProfile *> _changedTimeProfiles;
    QList<TimeProfile *> _removedTimeProfiles;

    QTimer *_updateTimer;
};

#endif // ROUTE_H
