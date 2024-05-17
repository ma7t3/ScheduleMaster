#ifndef CMDLINES_H
#define CMDLINES_H

#include <QUndoCommand>

#include "ProjectData/projectdata.h"
#include "Commands/CmdGeneral.h"

class CmdLineNew : public CmdAbstract {

public:
    CmdLineNew(ProjectData *d, Line *l) :
        CmdAbstract(QObject::tr("new line: %1").arg(l->name()), nullptr, LinesType),
        d(d),
        line(l)
    {}

    void undo() override {
        d->removeLine(line);
    }

    void redo() override {
        d->addLine(line);
        line->refreshChilds();
    }

private:
    ProjectData *d;
    Line *line;
};

class CmdLineChangeHourBreak: public CmdAbstract {

public:
    CmdLineChangeHourBreak(Line *l, int newHourBreak) :
        CmdAbstract(QObject::tr("change hour break of line: %1").arg(l->name()), nullptr, ScheduleHourBreakType),
        line(l),
        oldValue(l->hourBreak()),
        newValue(newHourBreak) {}

    void undo() override {
        line->setHourBreak(oldValue);
    }

    void redo() override {
        line->setHourBreak(newValue);
    }

private:
    Line *line;
    int oldValue, newValue;
};

class CmdLineEdit: public CmdAbstract {

public:
    CmdLineEdit(Line *l, Line newL) :
        CmdAbstract(QObject::tr("edit line: %1").arg(newL.name()), nullptr, LinesType),
        line(l),
        oldL(*l),
        newL(newL) {
        oldL.setDirections(oldL.cloneDirections());
    }

    void undo() override {
        *line = oldL;
    }

    void redo() override {
        *line = newL;
        line->refreshChilds();
    }

private:
    Line *line;
    Line oldL, newL;
};

class CmdLineDelete : public CmdAbstract {

public:
    CmdLineDelete(ProjectData *d, Line *l) :
        CmdAbstract(QObject::tr("delete line: %1").arg(l->name()), nullptr, LinesType),
        d(d),
        line(l)
    {}

    void undo() override {
        d->addLine(line);
    }

    void redo() override {
        d->removeLine(line);
    }

private:
    ProjectData *d;
    Line *line;
};


class CmdLinesDelete : public CmdAbstract {

public:
    CmdLinesDelete(ProjectData *d, QList<Line *> list) :
        CmdAbstract(list.count() == 1 ? QObject::tr("deleted line: %1").arg(list[0]->name()) : QObject::tr("deleted %n lines", "", list.count()), nullptr, LinesType),
        d(d),
        lines(list)
    {}

    void undo() override {
        for(int i = 0; i < lines.count(); i++)
            d->addLine(lines[i]);
    }

    void redo() override {
        for(int i = 0; i < lines.count(); i++)
            d->removeLine(lines[i]);
    }

private:
    ProjectData *d;
    QList<Line *> lines;
};

#endif // CMDLINES_H
