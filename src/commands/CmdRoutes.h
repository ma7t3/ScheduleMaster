#ifndef CMDROUTES_H
#define CMDROUTES_H

#include "CmdGeneral.h"
#include "ProjectData/Line.h"

class CmdRouteNew : public Command {
public:
    CmdRouteNew(Line *l, Route *r) :
        Command(QObject::tr("New route: %1").arg(r->name())),
        l(l),
        r(r) {}

    void redo() override {
        l->addRoute(r);
    }

    void undo() override {
        l->removeRoute(r);
    }

private:
    Line *l;
    Route *r;
};

class CmdRouteEdit : public Command {
public:
    CmdRouteEdit(Route *r, Route *newR) :
        Command(QObject::tr("Edit route: %1").arg(r->name())),
        r(r) {
        this->oldR = r->clone();
        this->newR = newR;
    }

    void redo() override {
        r->mergeData(newR->data());
    }

    void undo() override {
        r->mergeData(oldR->data());
    }

private:
    Route *r, *oldR, *newR;
};

class CmdRoutesRemove : public Command {
public:
    CmdRoutesRemove(Line *l, PDISet<Route> routes) :
        Command(routes.size() > 1
                    ? QObject::tr("Remove %n routes", "", routes.size())
                    : QObject::tr("Remove route: %1").arg(routes.values().first()->name())),
        l(l), routes(routes) {}

    void redo() override {
        for(Route *r : std::as_const(routes))
            l->removeRoute(r);
    }

    void undo() override {
        for(Route *r : std::as_const(routes))
            l->addRoute(r);
    }

private:
    Line *l;
    PDISet<Route> routes;
};

#endif // CMDROUTES_H
