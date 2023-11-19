#ifndef CMDROUTES_H
#define CMDROUTES_H

#include <QUndoCommand>

#include "ProjectData/line.h"
#include "ProjectData/route.h"

class cmdRouteNew : public QUndoCommand {

public:
    cmdRouteNew(Line *l, Route *r) :
        line(l),
        route(r) {
        setText(QObject::tr("new route %1 in line %2").arg(route->name()).arg(line->name()));
    }

    void undo() override {
        line->removeRoute(route);
    }

    void redo() override {
        line->addRoute(route);
    }

private:
    Line *line;
    Route *route;
};

class cmdRouteEdit : public QUndoCommand {

public:
    cmdRouteEdit(Route *r, Route newR
    ) :
        route(r),
        oldRoute(*r),
        newRoute(newR) {
        setText(QObject::tr("edit route: %1").arg(newR.name()));
    }

    void undo() override {
        route->setCode(oldRoute.code());
        route->setDirection(oldRoute.direction());
        route->setName(oldRoute.name());
        route->setBusstopList(oldRoute.busstops());
        route->setTimeProfileList(oldRoute.timeProfiles());
    }

    void redo() override {
        route->setCode(newRoute.code());
        route->setDirection(newRoute.direction());
        route->setName(newRoute.name());
        route->setBusstopList(newRoute.busstops());
        route->setTimeProfileList(newRoute.timeProfiles());
    }

private:
    Route *route;
    Route oldRoute, newRoute;
};

class cmdRouteDelete : public QUndoCommand {

public:
    cmdRouteDelete(Line *l, Route *r) :
        line(l),
        route(r) {
        setText(QObject::tr("delete route %1 from line %2").arg(route->name()).arg(line->name()));
    }

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

class cmdRoutesDelete : public QUndoCommand {

public:
    cmdRoutesDelete(Line *l, QList<Route *> list) :
        line(l),
        routes(list) {
        if(list.count() == 1)
            setText(QObject::tr("delete route %1 from line %2").arg(list[0]->name()).arg(line->name()));
        else
            setText(QObject::tr("delete %n routes  from line %2", "", list.count()).arg(line->name()));
    }

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
