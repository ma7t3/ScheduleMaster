#include "ProjectData.h"

ProjectData::ProjectData(QObject *parent) :
    QObject(parent),
    _undoStack(new QUndoStack(this)) {

}

QString ProjectData::filePath() const {
    return _filePath;
}

bool ProjectData::isKnownFile() const {
    return !_filePath.isEmpty();
}

void ProjectData::setFilePath(const QString &newFilePath) {
    _filePath = newFilePath;
}

QUndoStack *ProjectData::undoStack() const {
    return _undoStack;
}

void ProjectData::reset() {
    _undoStack->clear();
    _filePath = "";
}
