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
    return _busstops.contains(id) ? _busstops[id] : nullptr;
}

Busstop *ProjectData::findBusstopByName(const QString &name) const {
    auto it = std::find_if(_busstops.begin(), _busstops.end(),
                           [name](Busstop* busstop) {
                               return busstop->name() == name;
                           });

    return (it != _busstops.end()) ? it.value() : nullptr;
}

QList<Busstop *> ProjectData::busstops() const {
    return _busstops.values();
}

void ProjectData::addBusstop(Busstop *busstop) {
    _busstops.insert(busstop->id(), busstop);
}

void ProjectData::removeBusstop(Busstop *busstop) {
    removeBusstop(busstop->id());
}

void ProjectData::removeBusstop(const QUuid &id) {
    _busstops.remove(id);
}

QJsonObject ProjectData::toJson() const {
    return {};
}

void ProjectData::fromJson(const QJsonObject &jsonObject) {
    QJsonArray jBusstops = jsonObject.value("busstops").toArray(QJsonArray());

    for(int i = 0; i < jBusstops.count(); i++) {
        Busstop *b = new Busstop(this, jBusstops[i].toObject());
        _busstops.insert(b->id(), b);
    }
}
