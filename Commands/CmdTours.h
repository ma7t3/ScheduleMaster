#ifndef CMDTOURS_H
#define CMDTOURS_H

#include <QUndoStack>

#include "ProjectData/projectdata.h"

class cmdTourNew : public QUndoCommand {
public:
    cmdTourNew(ProjectData *d, Tour *o) :
        d(d),
        tour(o) {
        setText(QString("new tour (%1)").arg(tour->name()));
    }

    void undo() override {
        d->removeTour(tour);
    }

    void redo() override {
        d->addTour(tour);
    }

private:
    ProjectData *d;
    Tour *tour;
};

class cmdTourEdit : public QUndoCommand {
public:
    cmdTourEdit(Tour *o, Tour newO) :
        tour(o),
        oldTour(*o),
        newTour(newO) {
        setText(QString("edit tour (%1)").arg(newTour.name()));
    }

    void undo() override {
        tour->setName(oldTour.name());
        tour->setWeekDays(*oldTour.weekDays());
    }

    void redo() override {
        tour->setName(newTour.name());
        tour->setWeekDays(*newTour.weekDays());
    }

private:
    Tour *tour;
    Tour oldTour, newTour;
};

class cmdTourTripAdd : public QUndoCommand {
public:
    cmdTourTripAdd(Tour *o, Trip *t, int i) :
        tour(o),
        trip(t),
        index(i) {
        setText(QString("Add trip (%1) to tour (%2)").arg(trip->route()->name(), tour->name()));
    }

    void undo() override {
        tour->removeTrip(trip);
    }

    void redo() override {
        tour->insertTripAt(trip, index);
    }

private:
    Tour *tour;
    Trip *trip;
    int index;
};

class cmdTourTripRemove : public QUndoCommand {
public:
    cmdTourTripRemove(Tour *o, Trip *t, int i) :
        tour(o),
        trip(t),
        index(i) {
        setText(QString("Remove trip (%1) from tour (%2)").arg(trip->route()->name(), tour->name()));
    }

    void undo() override {
        tour->insertTripAt(trip, index);
    }

    void redo() override {
        tour->removeTrip(trip);
    }

private:
    Tour *tour;
    Trip *trip;
    int index;
};

class cmdTourDelete : public QUndoCommand {
public:
    cmdTourDelete(ProjectData *d, Tour *o) :
        d(d),
        tour(o) {
        setText(QString("delete tour (%1)").arg(tour->name()));
    }

    void undo() override {
        d->addTour(tour);
    }

    void redo() override {
        d->removeTour(tour);
    }

private:
    ProjectData *d;
    Tour *tour;
};

class cmdTourReorderTrips : public QUndoCommand {
public:
    cmdTourReorderTrips(Tour *o) :
        tour(o) {
        setText(QString("reorder trips of trip (%1)").arg(tour->name()));
        trips = tour->trips();
    }

    void undo() override {
        tour->setTripList(trips);
    }

    void redo() override {
        tour->setTripList(ProjectData::sortTrips(tour->trips()));
    }

private:
    Tour *tour;
    QList<Trip *> trips;
};

#endif // CMDTOURS_H
