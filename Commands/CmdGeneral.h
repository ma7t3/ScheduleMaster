#ifndef CMDGENERAL_H
#define CMDGENERAL_H

#include <QUndoStack>

#include "ProjectData/projectdata.h"

class CmdAbstract : QUndoCommand {
public:
    CmdAbstract(ProjectData *d) : d(d) {}

protected:
    ProjectData *d;
};

class CmdEditProjectSettings : public QUndoCommand {

public:
    CmdEditProjectSettings(ProjectSettings *s, ProjectSettings newS) :
        s(s),
        newS(newS),
        oldS(*s) {
        setText(QObject::tr("project settings edited"));

        // copy day types
        QList<DayType *> dayTypeCopies;
        for(int i = 0; i < oldS.dayTypeCount(); i++)
            dayTypeCopies << new DayType(*oldS.dayTypeAt(i));
        oldS.setDayTypes(dayTypeCopies);
    }

    void undo() override {
        *s = oldS;
    }

    void redo() override {
        *s = newS;
    }

private:
    ProjectSettings *s;
    ProjectSettings newS, oldS;
};

#endif // CMDGENERAL_H
