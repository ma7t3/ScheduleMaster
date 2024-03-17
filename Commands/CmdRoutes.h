#ifndef CMDROUTES_H
#define CMDROUTES_H

#include <QUndoCommand>

#include "ProjectData/line.h"
#include "ProjectData/route.h"
#include "Commands/CmdGeneral.h"

class CmdRouteNew : public CmdAbstract {

public:
    CmdRouteNew(Line *l, Route *r) :
        CmdAbstract(QObject::tr("new route %1 in line %2").arg(r->name()).arg(l->name()), nullptr, RoutesType),
        line(l),
        route(r)
    {}

    void undo() override {
        line->removeRoute(route);
    }

    void redo() override {
        line->addRoute(route);
        line->refreshChilds();
        route->refreshChilds();
    }

private:
    Line *line;
    Route *route;
};

class CmdRouteEdit : public CmdAbstract {

public:
    CmdRouteEdit(Route *r, Route newR) :
        CmdAbstract(QObject::tr("edit route: %1").arg(newR.name()), nullptr, RoutesType),
        route(r),
        oldRoute(*r),
        newRoute(newR)
    {}

    void undo() override {
        *route = oldRoute;
    }

    void redo() override {
        *route = newRoute;
        route->refreshChilds();
    }

private:
    Route *route;
    Route oldRoute, newRoute;
};

class CmdRouteDelete : public CmdAbstract {

public:
    CmdRouteDelete(Line *l, Route *r) :
        CmdAbstract(QObject::tr("delete route %1 from line %2").arg(r->name()).arg(l->name()), nullptr, RoutesType),
        line(l),
        route(r)
    {}

    void undo() override {
        line->addRoute(route);
    }

    void redo() override {
        line->removeRoute(route);
    }

private:
    Line *line;
    Route *route;
};

class CmdRoutesDelete : public CmdAbstract {

public:
    CmdRoutesDelete(Line *l, QList<Route *> list) :
        CmdAbstract(list.count() == 1 ? QObject::tr("delete route %1 from line %2").arg(list[0]->name()).arg(l->name()) : QObject::tr("delete %n routes  from line %2", "", list.count()).arg(l->name()), nullptr, RoutesType),
        line(l),
        routes(list)
    {}

    void undo() override {
        for(int i = 0; i < routes.count(); i++)
            line->addRoute(routes[i]);
    }

    void redo() override {
        for(int i = 0; i < routes.count(); i++)
            line->removeRoute(routes[i]);
    }

private:
    Line *line;
    QList<Route *> routes;
};

#endif // CMDROUTES_H
