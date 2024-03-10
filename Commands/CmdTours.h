#ifndef CMDTOURS_H
#define CMDTOURS_H

#include <QUndoStack>

#include "ProjectData/projectdata.h"

class CmdTourNew : public QUndoCommand {

public:
    CmdTourNew(ProjectData *d, Tour *o) :
        d(d),
        tour(o) {
        setText(QObject::tr("new tour: %1").arg(tour->name()));
    }

    void undo() override {
        d->removeTour(tour);
    }

    void redo() override {
        d->addTour(tour);
        tour->refreshChilds();
    }

private:
    ProjectData *d;
    Tour *tour;
};

class CmdTourEdit : public QUndoCommand {

public:
    CmdTourEdit(Tour *o, Tour newO) :
        tour(o),
        oldTour(*o),
        newTour(newO) {
        setText(QObject::tr("edit tour: %1").arg(newTour.name()));
    }

    void undo() override {
        *tour = oldTour;
    }

    void redo() override {
        *tour = newTour;
        tour->refreshChilds();
    }

private:
    Tour *tour;
    Tour oldTour, newTour;
};

class CmdTourTripAdd : public QUndoCommand {

public:
    CmdTourTripAdd(Tour *o, Trip *t, int i) :
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
        tour->refreshChilds();
    }

private:
    Tour *tour;
    Trip *trip;
    int index;
};

class CmdTourTripRemove : public QUndoCommand {

public:
    CmdTourTripRemove(Tour *o, Trip *t, int i) :
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

class CmdTourDelete : public QUndoCommand {

public:
    CmdTourDelete(ProjectData *d, Tour *o) :
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

class CmdToursDelete : public QUndoCommand {

public:
    CmdToursDelete(ProjectData *d, QList<Tour *> list) :
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

class CmdTourReorderTrips : public QUndoCommand {

public:
    CmdTourReorderTrips(Tour *o) :
        tour(o) {
        setText(QObject::tr("reorder trips of tour: %1").arg(tour->name()));
        trips = tour->trips();
    }

    void undo() override {
        tour->setTrips(trips);
    }

    void redo() override {
        tour->setTrips(ProjectData::sortItems(tour->trips()));
    }

private:
    Tour *tour;
    QList<Trip *> trips;
};

#endif // CMDTOURS_H
