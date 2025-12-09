#include "ApplicationInterface.h"

#include "ProjectData/ProjectData.h"

#include <QUndoStack>

void ApplicationInterface::init(ProjectData *projectData) {
    _projectData = projectData;
}

ApplicationInterface *ApplicationInterface::instance() {
    static ApplicationInterface instance;
    return &instance;
}

ProjectData *ApplicationInterface::projectData() {
    return _projectData;
}

QUndoStack *ApplicationInterface::undoStack() {
    return _projectData->undoStack();
}
