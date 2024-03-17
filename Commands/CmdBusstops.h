#ifndef CMDS_BUSSTOPS
#define CMDS_BUSSTOPS

#include <QUndoStack>

#include "ProjectData/projectdata.h"
#include "Commands/CmdGeneral.h"

class CmdBusstopNew : public CmdAbstract {

public:
    CmdBusstopNew(ProjectData *d, Busstop *b) :
        CmdAbstract(QObject::tr("new busstop: %1").arg(b->name()), nullptr, BusstopsType),
        d(d),
        busstop(b)
    {}

    void undo() override {
        d->removeBusstop(busstop);
    }

    void redo() override {
        d->addBusstop(busstop);
    }

private:
    ProjectData *d;
    Busstop *busstop;
};

class CmdBusstopEdit : public CmdAbstract {

public:
    CmdBusstopEdit(Busstop *b, Busstop newB) :
        CmdAbstract(QObject::tr("edit busstop (%1)").arg(newB.name()), nullptr, BusstopsType),
        busstop(b),
        oldB(*b),
        newB(newB)
    {}

    void undo() override {
        *busstop = oldB;
    }

    void redo() override {
        *busstop = newB;
    }

private:
    Busstop *busstop;
    Busstop oldB, newB;
};

class CmdBusstopDelete : public CmdAbstract {

public:
    CmdBusstopDelete(ProjectData *d, Busstop *b) :
        CmdAbstract(QObject::tr("delete busstop: %1").arg(b->name()), nullptr, BusstopsType),
        d(d),
        busstop(b)
    {}

    void undo() override {
        d->addBusstop(busstop);
    }

    void redo() override {
        d->removeBusstop(busstop);
    }

private:
    ProjectData *d;
    Busstop *busstop;
};

class CmdBusstopsDelete : public CmdAbstract {

public:
    CmdBusstopsDelete(ProjectData *d, QList<Busstop *> list) :
        CmdAbstract(list.count() == 1 ? QObject::tr("deleted busstop: %1").arg(list[0]->name()) : QObject::tr("deleted %n busstops", "", list.count()), nullptr, BusstopsType),
        d(d),
        busstops(list) {
    }

    void undo() override {
        for(int i = 0; i < busstops.count(); i++)
            d->addBusstop(busstops[i]);
    }

    void redo() override {
        for(int i = 0; i < busstops.count(); i++)
            d->removeBusstop(busstops[i]);
    }

private:
    ProjectData *d;
    QList<Busstop *> busstops;
};

#endif // CMDS_BUSSTOPS
