#ifndef CMDTOURS_H
#define CMDTOURS_H

#include <QUndoStack>

#include "ProjectData/projectdata.h"

class cmdTourNew : public QUndoCommand {

public:
    cmdTourNew(ProjectData *d, Tour *o) :
        d(d),
        tour(o) {
        setText(QObject::tr("new tour: %1").arg(tour->name()));
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
        setText(QObject::tr("edit tour: %1").arg(newTour.name()));
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
        setText(QObject::tr("Add trip %1 to tour %2").arg(trip->route()->name(), tour->name()));
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
        setText(QObject::tr("Remove trip %1 from tour %2").arg(trip->route()->name(), tour->name()));
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
        setText(QObject::tr("delete tour: %1").arg(tour->name()));
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

class cmdToursDelete : public QUndoCommand {

public:
    cmdToursDelete(ProjectData *d, QList<Tour *> list) :
        d(d),
        tours(list) {
        if(list.count() == 1)
            setText(QObject::tr("deleted tour: %1").arg(list[0]->name()));
        else
            setText(QObject::tr("deleted %n tours", "", list.count()));
    }

    void undo() override {
        for(int i = 0; i < tours.count(); i++)
            d->addTour(tours[i]);
    }

    void redo() override {
        for(int i = 0; i < tours.count(); i++)
            d->removeTour(tours[i]);
    }

private:
    ProjectData *d;
    QList<Tour *> tours;
};

class cmdTourReorderTrips : public QUndoCommand {

public:
    cmdTourReorderTrips(Tour *o) :
        tour(o) {
        setText(QObject::tr("reorder trips of tour: %1").arg(tour->name()));
        trips = tour->trips();
    }

    void undo() override {
        tour->setTrips(trips);
    }

    void redo() override {
        tour->setTrips(ProjectData::sortTrips(tour->trips()));
    }

private:
    Tour *tour;
    QList<Trip *> trips;
};

#endif // CMDTOURS_H
