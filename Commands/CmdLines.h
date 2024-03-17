#ifndef CMDLINES_H
#define CMDLINES_H

#include <QUndoCommand>

#include "ProjectData/projectdata.h"

class CmdLineNew : public QUndoCommand {

public:
    CmdLineNew(ProjectData *d, Line *l) :
        d(d),
        line(l) {
        setText(QObject::tr("new line: %1").arg(line->name()));
    }

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

class CmdLineEdit: public QUndoCommand {

public:
    CmdLineEdit(Line *l, Line newL) :
        line(l),
        oldL(*l),
        newL(newL) {
        setText(QObject::tr("edit line: %1").arg(newL.name()));
        // copy directions
        QList<LineDirection *> directionCopies;
        for(int i = 0; i < oldL.directionCount(); i++)
            directionCopies << new LineDirection(*oldL.directionAt(i));
        oldL.setDirections(directionCopies);
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

class CmdLineDelete : public QUndoCommand {

public:
    CmdLineDelete(ProjectData *d, Line *l) :
        d(d),
        line(l) {
        setText(QObject::tr("delete line: %1").arg(line->name()));
    }

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


class CmdLinesDelete : public QUndoCommand {

public:
    CmdLinesDelete(ProjectData *d, QList<Line *> list) :
        d(d),
        lines(list) {
        if(list.count() == 1)
            setText(QObject::tr("deleted line: %1").arg(list[0]->name()));
        else
            setText(QObject::tr("deleted %n lines", "", list.count()));
    }

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
