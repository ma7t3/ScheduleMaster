#include "ProjectData.h"

#include <QJsonArray>

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

int ProjectData::busstopCount() const {
    return _busstops.count();
}

Busstop *ProjectData::busstop(const QUuid &id) const {
    return _busstops.find(id);
}

Busstop *ProjectData::findBusstopByName(const QString &name) const {
    return _busstops.filterOne([name](Busstop *b){return b->name() == name;});
}

PDISet<Busstop> ProjectData::busstops() const {
    return _busstops;
}

void ProjectData::addBusstop(Busstop *busstop) {
    _busstops.add(busstop);
}

void ProjectData::removeBusstop(Busstop *busstop) {
    _busstops.remove(busstop);
}

void ProjectData::removeBusstop(const QUuid &id) {
    _busstops.remove(id);
}

Busstop *ProjectData::busstopOfPlatform(BusstopPlatform *busstopPlatform) {
    return _busstops.filterOne([busstopPlatform](Busstop *b){return b->platforms().contains(busstopPlatform);});
}

QJsonObject ProjectData::toJson() const {
    // TODO: implement
    return {};
}

void ProjectData::fromJson(const QJsonObject &jsonObject) {
    QJsonArray jBusstops = jsonObject.value("busstops").toArray();

    for(int i = 0; i < jBusstops.count(); i++) {
        Busstop *b = new Busstop(this, jBusstops[i].toObject());
        _busstops.insert(b->id(), b);
    }
}
