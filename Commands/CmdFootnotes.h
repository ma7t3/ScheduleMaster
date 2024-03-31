#ifndef CMDFOOTNOTES_H
#define CMDFOOTNOTES_H

#include <QUndoStack>

#include "ProjectData/projectdata.h"
#include "Commands/CmdGeneral.h"

class CmdFootnoteNew : public CmdAbstract {

public:
    CmdFootnoteNew(ProjectData *d, Footnote *f) :
        CmdAbstract(QObject::tr("new footnote: %1").arg(f->identifier()), nullptr, FootnotesType),
        d(d),
        footnote(f)
    {}

    void undo() override {
        qDebug() << "before";
        d->removeFootnote(footnote);
        qDebug() << "after";
    }

    void redo() override {
        d->addFootnote(footnote);
    }

private:
    ProjectData *d;
    Footnote *footnote;
};


class CmdFootnoteEdit : public CmdAbstract {

public:
    CmdFootnoteEdit(Footnote *f, Footnote newF) :
        CmdAbstract(QObject::tr("edit footnote: %1").arg(newF.identifier()), nullptr, FootnotesType),
        footnote(f),
        oldF(*f),
        newF(newF)
    {}

    void undo() override {
        *footnote = oldF;
    }

    void redo() override {
        *footnote = newF;
    }

private:
    Footnote *footnote;
    Footnote oldF, newF;
};


class CmdFootnoteDelete : public CmdAbstract {

public:
    CmdFootnoteDelete(ProjectData *d, Footnote *f) :
        CmdAbstract(QObject::tr("delete footnote: %1").arg(f->identifier()), nullptr, FootnotesType),
        d(d),
        footnote(f)
    {}

    void undo() override {
        d->addFootnote(footnote);
    }

    void redo() override {
        d->removeFootnote(footnote);
    }

private:
    ProjectData *d;
    Footnote *footnote;
};

class CmdFootnotesDelete : public CmdAbstract {

public:
    CmdFootnotesDelete(ProjectData *d, QList<Footnote *> list) :
        CmdAbstract(QObject::tr("delete %n footnote(s)", "", list.count()), nullptr, FootnotesType),
        d(d),
        list(list)
    {}

    void undo() override {
        foreach(Footnote *f, list)
            d->addFootnote(f);
    }

    void redo() override {
        foreach(Footnote *f, list)
            d->removeFootnote(f);
    }

private:
    ProjectData *d;
    QList<Footnote *> list;
};

#endif // CMDFOOTNOTES_H
