#ifndef CMDBUSSTOPS_H
#define CMDBUSSTOPS_H

#include "CmdGeneral.h"
#include "src/projectdata/ProjectData.h"

class CmdBusstopNew : public Command {
public:
    CmdBusstopNew(ProjectData *projectData, Busstop *b) :
        Command(QObject::tr("New busstop: %1").arg(b->name())),
        projectData(projectData),
        b(b) {}

    void redo() override {
        projectData->addBusstop(b);
    }

    void undo() override {
        projectData->removeBusstop(b);
    }

private:
    ProjectData *projectData;
    Busstop *b;
};

class CmdBusstopEdit : public Command {
public:
    CmdBusstopEdit(Busstop *b, Busstop *newB) :
        Command(QObject::tr("Edit busstop: %1").arg(b->name())),
        b(b) {
        this->oldB = b->clone();
        this->newB = newB;
    }

    void redo() override {
        b->mergeData(newB->data());
    }

    void undo() override {
        b->mergeData(oldB->data());
    }

private:
    Busstop *b, *oldB, *newB;
};

class CmdBusstopsRemove : public Command {
public:
    CmdBusstopsRemove(ProjectData *projectData, PDISet<Busstop> busstops) :
        Command(busstops.size() > 1 ? QObject::tr("Remove %n busstops", "", busstops.size()) : QObject::tr("Remove busstop: %1").arg(busstops.values().first()->name())),
        projectData(projectData),
        busstops(busstops) {}

    void redo() override {
        for(Busstop *b : std::as_const(busstops))
            projectData->removeBusstop(b);
    }

    void undo() override {
        for(Busstop *b : std::as_const(busstops))
            projectData->addBusstop(b);
    }

private:
    ProjectData *projectData;
    PDISet<Busstop> busstops;
};

#endif // CMDBUSSTOPS_H
