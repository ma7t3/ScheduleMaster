#ifndef CMDGENERAL_H
#define CMDGENERAL_H

#include <QUndoStack>

#include "ProjectData/projectsettings.h"

enum CmdType {
    GeneralType,
    BusstopsType,
    LinesType,
    RoutesType,
    ScheduleType,
    ToursType,
    PublicationsType,
    FootnotesType
};

class CmdAbstract : public QUndoCommand {
public:
    CmdAbstract(const QString &text, QUndoCommand *parent = nullptr, CmdType t = GeneralType) :
        QUndoCommand(text, parent),
        t(t)
    {}

protected:
    CmdType t;
};

class CmdEditProjectSettings : public CmdAbstract {

public:
    CmdEditProjectSettings(ProjectSettings *s, ProjectSettings newS, QUndoCommand *parent = nullptr) :
        CmdAbstract(QObject::tr("project settings edited"), parent, GeneralType),
        s(s),
        newS(newS),
        oldS(*s) {
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
