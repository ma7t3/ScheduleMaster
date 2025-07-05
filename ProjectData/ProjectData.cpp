#include "ProjectData.h"

#include <QJsonArray>
#include <QUndoStack>

ProjectData::ProjectData(QObject *parent) :
    QObject(parent),
    _undoStack(new QUndoStack(this)),
    _loadingJson(false) {
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
    _lines.clear();
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

int ProjectData::lineCount() const {
    return _lines.count();
}

Line *ProjectData::line(const QUuid &id) const {
    return _lines.value(id, nullptr);
}

Line *ProjectData::findLineByName(const QString &name) const {
    return _lines.filterOne([name](Line *l) { return l->name() == name; });
}

PDISet<Line> ProjectData::lines() const {
    return _lines;
}

Line *ProjectData::createLine(QObject *parent) {
    return new Line(parent ? parent : this);
}

Line *ProjectData::createLine(const QJsonObject &jsonObject) {
    return new Line(this, jsonObject);
}

void ProjectData::addLine(Line *line) {
    _lines.add(line);
    emit lineAdded(line);
}

void ProjectData::removeLine(Line *line) {
    _lines.remove(line);
    emit lineRemoved(line);
}

void ProjectData::removeLine(const QUuid &id) {
    Line *l = line(id);
    removeLine(l);
}

bool ProjectData::isLoadingJson() const {
    return _loadingJson;
}

QJsonObject ProjectData::toJson() const {
    // TODO: implement
    return {};
}

bool ProjectData::setJson(const QJsonObject &jsonObject, std::function<bool()> cancelRequested) {
    _loadingJson = true;

    QJsonArray jBusstops = jsonObject.value("busstops").toArray();
    QJsonArray jLines = jsonObject.value("lines").toArray();
    const int busstopCount = jBusstops.count();
    const int lineCount = jLines.count();

    emit progressMaximum(busstopCount);

    for(int i = 0; i < busstopCount; i++) {
        if(cancelRequested()) {
            reset();
            _loadingJson = false;
            return false;
        }

        Busstop *b = createBusstop(jBusstops[i].toObject());
        addBusstop(b);
        if(i % 10 == 0) // we don't need to update for every single busstop :)
            emit progressUpdate(i + 1);
    }

    emit progressMaximum(lineCount);
    for(int i = 0; i < lineCount; i++) {
        if(cancelRequested()) {
            reset();
            _loadingJson = false;
            return false;
        }

        Line *l = createLine(jLines[i].toObject());
        addLine(l);
        emit progressUpdate(i + 1);
    }

    _loadingJson = false;
    return true;
}
