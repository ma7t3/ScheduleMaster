#include "ProjectData.h"

#include <QJsonArray>
#include <QUndoStack>

ProjectData::ProjectData(QObject *parent) :
    QObject(parent),
    _undoStack(new QUndoStack(this)) {
    setObjectName(Global::ProjectDataRootObjectName);

    _busstops.setParentOwnsItems(true);
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
    _filePath.clear();
    _busstops.clear();
    emit cleared();
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

Busstop *ProjectData::createBusstop(QObject *parent) {
    return new Busstop(parent ? parent : this);
}

Busstop *ProjectData::createBusstop(const QJsonObject &jsonObject) {
    return new Busstop(this, jsonObject);
}

void ProjectData::addBusstop(Busstop *busstop) {
    _busstops.add(busstop);
    emit busstopAdded(busstop);
}

void ProjectData::removeBusstop(Busstop *busstop) {
    _busstops.remove(busstop);
    emit busstopRemoved(busstop);
}

void ProjectData::removeBusstop(const QUuid &id) {
    Busstop *b = _busstops.value(id, nullptr);
    _busstops.remove(id);
    if(b)
        emit busstopRemoved(b);
}

Busstop *ProjectData::busstopOfPlatform(BusstopPlatform *busstopPlatform) {
    return _busstops.filterOne([busstopPlatform](Busstop *b){return b->platforms().contains(busstopPlatform);});
}

QJsonObject ProjectData::toJson() const {
    // TODO: implement
    return {};
}

bool ProjectData::setJson(const QJsonObject &jsonObject, std::function<bool()> cancelRequested) {
    QJsonArray jBusstops = jsonObject.value("busstops").toArray();
    const int busstopCount = jBusstops.count();

    emit progressMaximum(busstopCount);

    for(int i = 0; i < busstopCount; i++) {
        if(cancelRequested()) {
            reset();
            return false;
        }

        Busstop *b = new Busstop(this, jBusstops[i].toObject());
        addBusstop(b);
        if(i % 10 == 0) // we don't need to update for every single busstop :)
            emit progressUpdate(i + 1);
    }
    return true;
}
