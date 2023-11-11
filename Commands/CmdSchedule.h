#ifndef CMDSCHEDULE_H
#define CMDSCHEDULE_H

#include <QUndoStack>

#include "ProjectData/projectdata.h"

class cmdScheduleTripNew : public QUndoCommand {
public:
    cmdScheduleTripNew(Line *l, Trip *t, bool direction) :
        line(l),
        trip(t),
        direction(direction) {
        setText(QString("new trip in line (%1)").arg(line->name()));
    }

    void undo() override {
        line->removeTrip(trip);
    }

    void redo() override {
        line->addTrip(trip);
    }

private:
    Line *line;
    Trip *trip;
    bool direction;
};


class cmdScheduleTripChangeRoute : public QUndoCommand {
public:
    cmdScheduleTripChangeRoute(Trip *t, Route *newR, TimeProfile *newP) :
        trip(t),
        oldRoute(t->route()),
        newRoute(newR),
        oldProfile(t->timeProfile()),
        newProfile(newP) {
        setText(QString("change route of trip (%1)").arg(newRoute->name()));
    }

    void undo() override {
        trip->setRoute(oldRoute);
        trip->setTimeProfile(oldProfile);
    }

    void redo() override {
        trip->setRoute(newRoute);
        trip->setTimeProfile(newProfile);
    }

private:
    Trip *trip;
    Route *oldRoute, *newRoute;
    TimeProfile *oldProfile, *newProfile;
};


class cmdScheduleTripChangeTimeProfile : public QUndoCommand {
public:
    cmdScheduleTripChangeTimeProfile(Trip *t, TimeProfile *newP) :
        trip(t),
        oldProfile(t->timeProfile()),
        newProfile(newP) {
        setText(QString("change time profile of trip"));
    }

    void undo() override {
        trip->setTimeProfile(oldProfile);
    }

    void redo() override {
        trip->setTimeProfile(newProfile);
    }

private:
    Trip *trip;
    TimeProfile *oldProfile, *newProfile;
};


class cmdScheduleTripChangeStartTime : public QUndoCommand {
public:
    cmdScheduleTripChangeStartTime(Trip *t, QTime oldStartTime, QTime newStartTime) :
        trip(t),
        oldStartTime(oldStartTime),
        newStartTime(newStartTime) {
        setText(QString("change start time of trip (%1)").arg(newStartTime.toString("hh:mm")));
    }

    void undo() override {
        trip->setStartTime(oldStartTime);
    }

    void redo() override {
        trip->setStartTime(newStartTime);
    }

private:
    Trip *trip;
    QTime oldStartTime, newStartTime;
};


class cmdScheduleTripChangeDays : public QUndoCommand {
public:
    cmdScheduleTripChangeDays(Trip *t, WeekDays w) :
        trip(t),
        oldWeekDays(*t->weekDays()),
        newWeekDays(w) {
        setText(QString("change days of trip (%1)").arg(newWeekDays.toString()));
    }

    void undo() override {
        trip->weekDays()->setCode(oldWeekDays.toCode());
        trip->refreshChilds();
    }

    void redo() override {
        trip->weekDays()->setCode(newWeekDays.toCode());
        trip->refreshChilds();
    }

private:
    Trip *trip;
    WeekDays oldWeekDays, newWeekDays;
};


class cmdScheduleTripEditRepeat : public QUndoCommand {
public:
    cmdScheduleTripEditRepeat(Trip *t, Trip newT) :
        trip(t),
        oldTrip(*t),
        newTrip(newT) {
        setText(QString("change repeat of trip"));
    }

    void undo() override {
        trip->setRepeat(oldTrip.hasRepeat());
        if(oldTrip.hasRepeat()) {
            trip->setRepeatInterval(oldTrip.repeatInterval());
            trip->setRepeatEnd(oldTrip.repeatEnd());
        }
    }

    void redo() override {
        trip->setRepeat(newTrip.hasRepeat());
        if(newTrip.hasRepeat()) {
            trip->setRepeatInterval(newTrip.repeatInterval());
            trip->setRepeatEnd(newTrip.repeatEnd());
        }
    }

private:
    Trip *trip;
    Trip oldTrip, newTrip;
};


class cmdScheduleTripDelete : public QUndoCommand {
public:
    cmdScheduleTripDelete(Line *l, Trip *t) :
        line(l),
        trip(t) {
        setText(QString("delete trip from line (%1)").arg(line->name()));
    }

    void undo() override {
        line->addTrip(trip);
    }

    void redo() override {
        line->removeTrip(trip);
    }

private:
    Line *line;
    Trip *trip;
};


#endif // CMDSCHEDULE_H
