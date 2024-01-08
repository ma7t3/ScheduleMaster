#ifndef CMDS_BUSSTOPS
#define CMDS_BUSSTOPS

#include <QUndoStack>

#include "ProjectData/projectdata.h"

class CmdBusstopNew : public QUndoCommand {

public:
    CmdBusstopNew(ProjectData *d, Busstop *b) :
        d(d),
        busstop(b) {
        setText(QObject::tr("new busstop: %1").arg(busstop->name()));
    }

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

class CmdBusstopEdit : public QUndoCommand {

public:
    CmdBusstopEdit(Busstop *b, Busstop newB) :
        busstop(b),
        oldB(*b),
        newB(newB) {
        setText(QObject::tr("edit busstop (%1)").arg(newB.name()));
    }

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

class CmdBusstopDelete : public QUndoCommand {

public:
    CmdBusstopDelete(ProjectData *d, Busstop *b) :
        d(d),
        busstop(b) {
        setText(QObject::tr("delete busstop: %1").arg(busstop->name()));
    }

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

class CmdBusstopsDelete : public QUndoCommand {

public:
    CmdBusstopsDelete(ProjectData *d, QList<Busstop *> list) :
        d(d),
        busstops(list) {
        if(list.count() == 1)
            setText(QObject::tr("deleted busstop: %1").arg(list[0]->name()));
        else
            setText(QObject::tr("deleted %n busstops", "", list.count()));
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
