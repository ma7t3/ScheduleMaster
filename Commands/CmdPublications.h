#ifndef CMDPUBLICATIONS_H
#define CMDPUBLICATIONS_H

#include <QUndoCommand>

#include "ProjectData/projectdata.h"
#include "Commands/CmdGeneral.h"

class CmdPublishedLineNew : public CmdAbstract {

public:
    CmdPublishedLineNew(ProjectData *d, PublishedLine *l) :
        CmdAbstract(QObject::tr("new published line: %1").arg(l->title()), nullptr, PublicationsType),
        d(d),
        line(l)
    {}

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

class CmdPublishedLineEdit : public CmdAbstract {

public:
    CmdPublishedLineEdit(PublishedLine *l, PublishedLine newL) :
        CmdAbstract(QObject::tr("published line edited: %1").arg(newL.title()), nullptr, PublicationsType),
        line(l),
        oldLine(*l),
        newLine(newL)
    {}

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

class CmdPublishedLineDelete: public CmdAbstract {

public:
    CmdPublishedLineDelete(ProjectData *d, PublishedLine *l) :
        CmdAbstract(QObject::tr("deleted published line: %1").arg(l->title()), nullptr, PublicationsType),
        d(d),
        line(l)
    {}

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

class CmdPublishedLineDirectionNew : public CmdAbstract {

public:
    CmdPublishedLineDirectionNew(PublishedLine *l, PublishedLineDirection *ld) :
        CmdAbstract(QObject::tr("new published line direction: %1").arg(ld->name()), nullptr, PublicationsType),
        line(l),
        lineDirection(ld)
    {}

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

class CmdPublishedLineDirectionEdit : public CmdAbstract {

public:
    CmdPublishedLineDirectionEdit(PublishedLineDirection *ld, PublishedLineDirection newLd) :
        CmdAbstract(QObject::tr("published line direction edited: %1").arg(newLd.name()), nullptr, PublicationsType),
        direction(ld),
        oldDirection(*ld),
        newDirection(newLd)
    {}

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

class CmdPublishedLineDirectionDelete: public CmdAbstract {

public:
    CmdPublishedLineDirectionDelete(PublishedLine *l, PublishedLineDirection *ld) :
        CmdAbstract(QObject::tr("deleted published line direction: %1").arg(ld->name()), nullptr, PublicationsType),
        line(l),
        lineDirection(ld)
    {}

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

class CmdPublishedBusstopEdit: public CmdAbstract {

public:
    CmdPublishedBusstopEdit(PublishedBusstop *b, PublishedBusstop newB) :
        CmdAbstract(QObject::tr("edit published busstop: %1").arg(newB.linkedBusstop()->name()), nullptr, PublicationsType),
        busstop(b),
        oldB(*b),
        newB(newB)
    {}

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
