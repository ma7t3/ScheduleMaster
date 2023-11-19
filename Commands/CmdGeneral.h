#ifndef CMDGENERAL_H
#define CMDGENERAL_H

#include <QUndoStack>

#include "ProjectData/projectdata.h"

class cmdAbstract : QUndoCommand {
public:
    cmdAbstract(ProjectData *d) : d(d) {}

protected:
    ProjectData *d;
};

class cmdEditProjectSettings : public QUndoCommand {

public:
    cmdEditProjectSettings(ProjectSettings *s, ProjectSettings newS) :
        s(s),
        newS(newS),
        oldS(*s) {
        setText(QObject::tr("project settings edited"));
    }

    void undo() override {
        s->setNames(oldS.displayName(), oldS.shortName());
        s->setIcon(oldS.icon());
        s->setDayTypes(oldS.dayTypes());
    }

    void redo() override {
        s->setNames(newS.displayName(), newS.shortName());
        s->setIcon(newS.icon());
        s->setDayTypes(newS.dayTypes());
    }

private:
    ProjectSettings *s;
    ProjectSettings newS, oldS;
};

#endif // CMDGENERAL_H
