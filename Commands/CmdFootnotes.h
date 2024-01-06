#ifndef CMDFOOTNOTES_H
#define CMDFOOTNOTES_H

#include <QUndoStack>

#include "ProjectData/projectdata.h"

class cmdFootnoteNew : public QUndoCommand {

public:
    cmdFootnoteNew(ProjectData *d, Footnote *f) :
        d(d),
        footnote(f) {
        setText(QObject::tr("new footnote: %1").arg(footnote->identifier()));
    }

    void undo() override {
        d->removeFootnote(footnote);
    }

    void redo() override {
        d->addFootnote(footnote);
    }

private:
    ProjectData *d;
    Footnote *footnote;
};


class cmdFootnoteEdit : public QUndoCommand {

public:
    cmdFootnoteEdit(Footnote *f, Footnote newF) :
        footnote(f),
        oldF(*f),
        newF(newF) {
        setText(QObject::tr("edit footnote: %1").arg(newF.identifier()));
    }

    void undo() override {
        footnote->overwrite(oldF);
    }

    void redo() override {
        footnote->overwrite(newF);
    }

private:
    Footnote *footnote;
    Footnote oldF, newF;
};


class cmdFootnoteDelete : public QUndoCommand {

public:
    cmdFootnoteDelete(ProjectData *d, Footnote *f) :
        d(d),
        footnote(f) {
        setText(QObject::tr("delete footnote: %1").arg(footnote->identifier()));
    }

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

#endif // CMDFOOTNOTES_H
