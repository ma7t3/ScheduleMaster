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
    }

private:
    PublishedBusstop *busstop;
    PublishedBusstop oldB, newB;
};


class CmdPublishedBusstopsEdit : public CmdAbstract {

public:
    CmdPublishedBusstopsEdit(QList<QPair<PublishedBusstop *, PublishedBusstop>> list) :
        CmdAbstract(list.count() == 1 ? QObject::tr("edit published busstop: %1").arg(list[0].second.linkedBusstop()->name()) : QObject::tr("published busstops edited"), nullptr, PublicationsType),
        data(list) {
        for(int i = 0; i < list.count(); i++) {
            oldData << *list[i].first;
        }
    }

    void undo() override {
        for(int i = 0; i < data.count(); i++) {
            *data[i].first = oldData[i];
        }
    }

    void redo() override {
        for(int i = 0; i < data.count(); i++) {
            *data[i].first = data[i].second;
        }
    }

private:
    QList<QPair<PublishedBusstop *, PublishedBusstop>> data;
    QList<PublishedBusstop> oldData;
};

#endif // CMDPUBLICATIONS_H
