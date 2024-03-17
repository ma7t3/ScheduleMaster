#ifndef CMDTOURS_H
#define CMDTOURS_H

#include <QUndoStack>

#include "ProjectData/projectdata.h"
#include "Commands/CmdGeneral.h"

class CmdTourNew : public CmdAbstract {

public:
    CmdTourNew(ProjectData *d, Tour *o) :
        CmdAbstract(QObject::tr("new tour: %1").arg(o->name()), nullptr, ToursType),
        d(d),
        tour(o)
    {}

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

class CmdTourEdit : public CmdAbstract {

public:
    CmdTourEdit(Tour *o, Tour newO) :
        CmdAbstract(QObject::tr("edit tour: %1").arg(newO.name()),  nullptr, ToursType),
        tour(o),
        oldTour(*o),
        newTour(newO)
    {}

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

class CmdTourTripAdd : public CmdAbstract {

public:
    CmdTourTripAdd(Tour *o, Trip *t, int i) :
        CmdAbstract(QObject::tr("Add trip %1 to tour %2").arg(t->route()->name(), o->name()), nullptr, ToursType),
        tour(o),
        trip(t),
        index(i)
    {}

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

class CmdTourTripRemove : public CmdAbstract {

public:
    CmdTourTripRemove(Tour *o, Trip *t, int i) :
        CmdAbstract(QObject::tr("Remove trip %1 from tour %2").arg(t->route()->name(), o->name()), nullptr, ToursType),
        tour(o),
        trip(t),
        index(i)
    {}

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

class CmdTourDelete : public CmdAbstract {

public:
    CmdTourDelete(ProjectData *d, Tour *o) :
        CmdAbstract(QObject::tr("delete tour: %1").arg(o->name()), nullptr, ToursType),
        d(d),
        tour(o)
    {}

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

class CmdToursDelete : public CmdAbstract {

public:
    CmdToursDelete(ProjectData *d, QList<Tour *> list) :
        CmdAbstract(list.count() == 1 ? QObject::tr("deleted tour: %1").arg(list[0]->name()) : QObject::tr("deleted %n tours", "", list.count()), nullptr, ToursType),
        d(d),
        tours(list)
    {}

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

class CmdTourReorderTrips : public CmdAbstract {

public:
    CmdTourReorderTrips(Tour *o) :
        CmdAbstract(QObject::tr("reorder trips of tour: %1").arg(o->name()), nullptr, ToursType),
        tour(o) {
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
