#ifndef CMDSCHEDULE_H
#define CMDSCHEDULE_H

#include <QUndoStack>

#include "ProjectData/line.h"
#include "Commands/CmdGeneral.h"

class CmdScheduleTripNew : public CmdAbstract {

public:
    CmdScheduleTripNew(Line *l, Trip *t) :
        CmdAbstract(QObject::tr("new trip in line %1").arg(l->name()), nullptr, ScheduleType),
        line(l),
        trip(t)
    {}

    void undo() override {
        line->removeTrip(trip);
    }

    void redo() override {
        line->addTrip(trip);
        line->refreshChilds();
        trip->refreshChilds();
    }

private:
    Line *line;
    Trip *trip;
};

class CmdScheduleTripsNew : public CmdAbstract {

public:
    CmdScheduleTripsNew(Line *l, QList<Trip *> list) :
        CmdAbstract(QObject::tr("new trips in line %1").arg(l->name()), nullptr, ScheduleType),
        line(l),
        trips(list)
    {}

    void undo() override {
        for(int i = 0; i < trips.count(); i++)
            line->removeTrip(trips[i]);
    }

    void redo() override {
        for(int i = 0; i < trips.count(); i++) {
            line->addTrip(trips[i]);
            trips[i]->refreshChilds();
        }
        line->refreshChilds();
    }

private:
    Line *line;
    QList<Trip *> trips;
};



class CmdScheduleTripsChangeRoute : public CmdAbstract {

public:
    CmdScheduleTripsChangeRoute(QList<Trip *> list, Route *newRoute) :
        CmdAbstract(QObject::tr("change route of trips"), nullptr, ScheduleType),
        trips(list),
        newRoute(newRoute) {
        for(int i = 0; i < trips.count(); i++) {
            oldRoutes << trips[i]->route();
            oldProfiles << trips[i]->timeProfile();
        }
    }

    void undo() override {
        for(int i = 0; i < trips.count(); i++) {
            Trip *t = trips[i];
            t->setRoute(oldRoutes[i]);
            t->setTimeProfile(oldProfiles[i]);
        }
    }

    void redo() override {
        for(int i = 0; i < trips.count(); i++) {
            Trip *t = trips[i];
            QString oldProfileName;
            TimeProfile *newProfile = newRoute->timeProfileWithName(oldProfileName);
            if(!newProfile)
                newProfile = newRoute->timeProfileAt(0);

            t->setRoute(newRoute);
            t->setTimeProfile(newProfile);
        }
    }

private:
    QList<Trip *> trips;
    QList<Route *> oldRoutes;
    QList<TimeProfile *> oldProfiles;
    Route *newRoute;
};


class CmdScheduleTripsChangeTimeProfile : public CmdAbstract {

public:
    CmdScheduleTripsChangeTimeProfile(QList<Trip *> list, QString profileName) :
        CmdAbstract(QObject::tr("change time profiles of trips"), nullptr, ScheduleType),
        trips(list),
        newProfileName(profileName) {
        for(int i = 0; i < trips.count(); i++)
            oldProfiles << trips[i]->timeProfile();
    }

    void undo() override {
        for(int i = 0; i < trips.count(); i++) {
            trips[i]->setTimeProfile(oldProfiles[i]);
        }
    }

    void redo() override {
        for(int i = 0; i < trips.count(); i++) {
            Trip *t = trips[i];
            TimeProfile *newProfile = t->route()->timeProfileWithName(newProfileName);
            if(!newProfile)
                continue;

            t->setTimeProfile(newProfile);
        }
    }

private:
    QList<Trip *> trips;
    QString newProfileName;
    QList<TimeProfile *> oldProfiles;
};


class CmdScheduleTripChangeStartTime : public CmdAbstract {

public:
    CmdScheduleTripChangeStartTime(QList<Trip *> list, QList<QTime> oldStartTimes, QList<QTime> newStartTimes) :
        CmdAbstract(QObject::tr("change start time of trips"), nullptr, ScheduleType),
        trips(list),
        oldStartTimes(oldStartTimes),
        newStartTimes(newStartTimes)
    {}

    void undo() override {
        for(int i = 0; i < trips.count(); i++) {
            trips[i]->setStartTime(oldStartTimes[i]);
        }
    }

    void redo() override {
        for(int i = 0; i < trips.count(); i++) {
            trips[i]->setStartTime(newStartTimes[i]);
        }
    }

private:
    QList<Trip *> trips;
    QList<QTime> oldStartTimes, newStartTimes;
};


class CmdScheduleTripsChangeDays : public CmdAbstract {

public:
    CmdScheduleTripsChangeDays(QList<Trip *> trips, WeekDays w) :
        CmdAbstract(QObject::tr("change days of trips"), nullptr, ScheduleType),
        trips(trips),
        newWeekDays(w) {
        for(int i = 0; i < trips.count(); i++) {
            oldWeekDays << trips[i]->weekDays();
        }
    }

    void undo() override {
        for(int i = 0; i < trips.count(); i++) {
            trips[i]->setWeekDays(oldWeekDays[i]);
        }
    }

    void redo() override {
        for(int i = 0; i < trips.count(); i++) {
            trips[i]->setWeekDays(newWeekDays);
        }
    }

private:
    QList<Trip *> trips;
    QList<WeekDays> oldWeekDays;
    WeekDays newWeekDays;
};


class CmdScheduleTripsDelete : public CmdAbstract {

public:
    CmdScheduleTripsDelete(Line *l, QList<Trip *> list) :
        CmdAbstract(QObject::tr("delete trips from line %1").arg(l->name()), nullptr, ScheduleType),
        line(l),
        trips(list)
    {}

    void undo() override {
        for(int i = 0; i < trips.count(); i++)
            line->addTrip(trips[i]);

    }

    void redo() override {
        for(int i = 0; i < trips.count(); i++)
            line->removeTrip(trips[i]);
    }

private:
    Line *line;
    QList<Trip *> trips;
};


#endif // CMDSCHEDULE_H
