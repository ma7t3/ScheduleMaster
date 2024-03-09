#ifndef CMDSCHEDULE_H
#define CMDSCHEDULE_H

#include <QUndoStack>

#include "ProjectData/projectdata.h"

class CmdScheduleTripNew : public QUndoCommand {

public:
    CmdScheduleTripNew(Line *l, Trip *t) :
        line(l),
        trip(t) {
        setText(QObject::tr("new trip in line %1").arg(line->name()));
    }

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

class CmdScheduleTripsNew : public QUndoCommand {

public:
    CmdScheduleTripsNew(Line *l, QList<Trip *> list) :
        line(l),
        trips(list) {
        setText(QObject::tr("new trips in line %1").arg(line->name()));
    }

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



class CmdScheduleTripsChangeRoute : public QUndoCommand {

public:
    CmdScheduleTripsChangeRoute(QList<Trip *> list, Route *newRoute) :
        trips(list),
        newRoute(newRoute) {
        setText(QObject::tr("change route of trips"));
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


class CmdScheduleTripsChangeTimeProfile : public QUndoCommand {

public:
    CmdScheduleTripsChangeTimeProfile(QList<Trip *> list, QString profileName) :
        trips(list),
        newProfileName(profileName) {
        setText(QObject::tr("change time profiles of trips"));

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


class CmdScheduleTripChangeStartTime : public QUndoCommand {

public:
    CmdScheduleTripChangeStartTime(QList<Trip *> list, QList<QTime> oldStartTimes, QList<QTime> newStartTimes) :
        trips(list),
        oldStartTimes(oldStartTimes),
        newStartTimes(newStartTimes) {
        setText(QObject::tr("change start time of trips"));
    }

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


class CmdScheduleTripsChangeDays : public QUndoCommand {

public:
    CmdScheduleTripsChangeDays(QList<Trip *> trips, WeekDays w) :
        trips(trips),
        newWeekDays(w) {
        setText(QObject::tr("change days of trips"));

        for(int i = 0; i < trips.count(); i++) {
            oldWeekDays << *trips[i]->weekDays();
        }
    }

    void undo() override {
        for(int i = 0; i < trips.count(); i++) {
            trips[i]->weekDays()->setCode(oldWeekDays[i].toCode());
        }
    }

    void redo() override {
        for(int i = 0; i < trips.count(); i++) {
            trips[i]->weekDays()->setCode(newWeekDays.toCode());
        }
    }

private:
    QList<Trip *> trips;
    QList<WeekDays> oldWeekDays;
    WeekDays newWeekDays;
};


class CmdScheduleTripsDelete : public QUndoCommand {

public:
    CmdScheduleTripsDelete(Line *l, QList<Trip *> list) :
        line(l),
        trips(list) {
        setText(QObject::tr("delete trips from line %1").arg(line->name()));
    }

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
