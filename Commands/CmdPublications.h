#ifndef CMDPUBLICATIONS_H
#define CMDPUBLICATIONS_H

#include <QUndoCommand>

#include "ProjectData/projectdata.h"

class cmdPublishedLineNew : public QUndoCommand {

public:
    cmdPublishedLineNew(ProjectData *d, PublishedLine *l) :
        d(d),
        line(l) {
        setText(QObject::tr("new published line: %1").arg(line->title()));
    }

    void undo() override {
        d->publications()->removeLine(line);
    }

    void redo() override {
        d->publications()->addLine(line);
    }

private:
    ProjectData *d;
    PublishedLine *line;
};

class cmdPublishedLineEdit : public QUndoCommand {

public:
    cmdPublishedLineEdit(PublishedLine *l, PublishedLine newL) :
        line(l),
        oldLine(*l),
        newLine(newL) {
        setText(QObject::tr("published line edited: %1").arg(newLine.title()));
    }

    void undo() override {
        line->setNew(oldLine);
    }

    void redo() override {
        line->setNew(newLine);
    }

private:
    PublishedLine *line;
    PublishedLine oldLine, newLine;
};

class cmdPublishedLineDelete: public QUndoCommand {

public:
    cmdPublishedLineDelete(ProjectData *d, PublishedLine *l) :
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

class cmdPublishedLineDirectionNew : public QUndoCommand {

public:
    cmdPublishedLineDirectionNew(PublishedLine *l, PublishedLineDirection *ld) :
        line(l),
        lineDirection(ld) {
        setText(QObject::tr("new published line direction: %1").arg(lineDirection->name()));
    }

    void undo() override {
        line->removeDirection(lineDirection);
    }

    void redo() override {
        line->addDirection(lineDirection);
    }

private:
    PublishedLine *line;
    PublishedLineDirection *lineDirection;
};

class cmdPublishedLineDirectionEdit : public QUndoCommand {

public:
    cmdPublishedLineDirectionEdit(PublishedLineDirection *ld, PublishedLineDirection newLd) :
        direction(ld),
        oldDirection(*ld),
        newDirection(newLd) {
        setText(QObject::tr("published line direction edited: %1").arg(newDirection.name()));
    }

    void undo() override {
        direction->setNew(oldDirection);
    }

    void redo() override {
        direction->setNew(newDirection);
    }

private:
    PublishedLineDirection *direction;
    PublishedLineDirection oldDirection, newDirection;
};

class cmdPublishedLineDirectionDelete: public QUndoCommand {

public:
    cmdPublishedLineDirectionDelete(PublishedLine *l, PublishedLineDirection *ld) :
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

class cmdPublishedBusstopEdit: public QUndoCommand {

public:
    cmdPublishedBusstopEdit(PublishedBusstop *b, PublishedBusstop newB) :
        busstop(b),
        oldB(*b),
        newB(newB) {
        setText(QObject::tr("edit published busstop: %1").arg(newB.linkedBusstop()->name()));
    }

    void undo() override {
        busstop->setNew(oldB);
    }

    void redo() override {
        busstop->setNew(newB);
    }

private:
    PublishedBusstop *busstop;
    PublishedBusstop oldB, newB;
};

#endif // CMDPUBLICATIONS_H
