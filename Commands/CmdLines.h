#ifndef CMDLINES_H
#define CMDLINES_H

#include "Commands/CmdGeneral.h"
#include "ProjectData/ProjectData.h"

class CmdLineNew : public Command {
public:
    CmdLineNew(ProjectData *projectData, Line *l) :
        Command(QObject::tr("New line: %1").arg(l->name())), projectData(projectData), l(l) {}

    void redo() override {
        projectData->addLine(l);
    }

    void undo() override {
        projectData->removeLine(l);
    }

private:
    ProjectData *projectData;
    Line *l;
};

class CmdLineEdit : public Command {
public:
    CmdLineEdit(Line *l, Line *newL) :
        Command(QObject::tr("Edit line: %1").arg(l->name())), l(l) {
        this->oldL = l->clone();
        this->newL = newL;
    }

    void redo() override {
        l->mergeData(newL->data());
    }

    void undo() override {
        l->mergeData(oldL->data());
    }

private:
    Line *l, *oldL, *newL;
};

class CmdLinesRemove : public Command {
public:
    CmdLinesRemove(ProjectData *projectData, PDISet<Line> lines) :
        Command(lines.size() > 1 ? QObject::tr("Remove %n lines", "", lines.size()) : QObject::tr("Remove busstop: %1").arg(lines.values().first()->name())),
        projectData(projectData),
        lines(lines) {}

    void redo() override {
        for(Line *l : std::as_const(lines))
            projectData->removeLine(l);
    }

    void undo() override {
        for(Line *l : std::as_const(lines))
            projectData->addLine(l);
    }

private:
    ProjectData *projectData;
    PDISet<Line> lines;
};

#endif // CMDLINES_H
