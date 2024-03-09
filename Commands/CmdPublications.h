#ifndef CMDPUBLICATIONS_H
#define CMDPUBLICATIONS_H

#include <QUndoCommand>

#include "ProjectData/projectdata.h"

class CmdPublishedLineNew : public QUndoCommand {

public:
    CmdPublishedLineNew(ProjectData *d, PublishedLine *l) :
        d(d),
        line(l) {
        setText(QObject::tr("new published line: %1").arg(line->title()));
    }

    void undo() override {
        d->publications()->removeLine(line);
    }

    void redo() override {
        d->publications()->addLine(line);
        d->publications()->refreshChilds();
        line->refreshChilds();
    }

private:
    ProjectData *d;
    PublishedLine *line;
};

class CmdPublishedLineEdit : public QUndoCommand {

public:
    CmdPublishedLineEdit(PublishedLine *l, PublishedLine newL) :
        line(l),
        oldLine(*l),
        newLine(newL) {
        setText(QObject::tr("published line edited: %1").arg(newLine.title()));
    }

    void undo() override {
        *line = oldLine;
    }

    void redo() override {
        *line = newLine;
        line->refreshChilds();
    }

private:
    PublishedLine *line;
    PublishedLine oldLine, newLine;
};

class CmdPublishedLineDelete: public QUndoCommand {

public:
    CmdPublishedLineDelete(ProjectData *d, PublishedLine *l) :
        d(d),
        line(l) {
        setText(QObject::tr("deleted published line: %1").arg(line->title()));
    }

    void undo() override {
        d->publications()->addLine(line);
    }

    void redo() override {
        d->publications()->removeLine(line);
    }

private:
    ProjectData *d;
    PublishedLine *line;
};

class CmdPublishedLineDirectionNew : public QUndoCommand {

public:
    CmdPublishedLineDirectionNew(PublishedLine *l, PublishedLineDirection *ld) :
        line(l),
        lineDirection(ld) {
        setText(QObject::tr("new published line direction: %1").arg(lineDirection->name()));
    }

    void undo() override {
        line->removeDirection(lineDirection);
    }

    void redo() override {
        line->addDirection(lineDirection);
        line->refreshChilds();
        lineDirection->refreshChilds();
    }

private:
    PublishedLine *line;
    PublishedLineDirection *lineDirection;
};

class CmdPublishedLineDirectionEdit : public QUndoCommand {

public:
    CmdPublishedLineDirectionEdit(PublishedLineDirection *ld, PublishedLineDirection newLd) :
        direction(ld),
        oldDirection(*ld),
        newDirection(newLd) {
        setText(QObject::tr("published line direction edited: %1").arg(newDirection.name()));
    }

    void undo() override {
        *direction = oldDirection;
    }

    void redo() override {
        *direction = newDirection;
        direction->refreshChilds();
    }

private:
    PublishedLineDirection *direction;
    PublishedLineDirection oldDirection, newDirection;
};

class CmdPublishedLineDirectionDelete: public QUndoCommand {

public:
    CmdPublishedLineDirectionDelete(PublishedLine *l, PublishedLineDirection *ld) :
        line(l),
        lineDirection(ld) {
        setText(QObject::tr("deleted published line direction: %1").arg(lineDirection->name()));
    }

    void undo() override {
        line->addDirection(lineDirection);
    }

    void redo() override {
        line->removeDirection(lineDirection);
    }

private:
    PublishedLine *line;
    PublishedLineDirection *lineDirection;
};

class CmdPublishedBusstopEdit: public QUndoCommand {

public:
    CmdPublishedBusstopEdit(PublishedBusstop *b, PublishedBusstop newB) :
        busstop(b),
        oldB(*b),
        newB(newB) {
        setText(QObject::tr("edit published busstop: %1").arg(newB.linkedBusstop()->name()));
    }

    void undo() override {
        *busstop = oldB;
    }

    void redo() override {
        *busstop = newB;
        busstop->refreshChilds();
    }

private:
    PublishedBusstop *busstop;
    PublishedBusstop oldB, newB;
};

#endif // CMDPUBLICATIONS_H
