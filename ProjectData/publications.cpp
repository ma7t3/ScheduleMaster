#include "publications.h"

Publications::Publications(QObject *parent) :
    ProjectDataItem(parent) {}

Publications::Publications(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

Publications::Publications(const Publications &other) :
    ProjectDataItem(other.parent()) {
    copy(other);
}


Publications Publications::operator=(const Publications &other) {
    copy(other);
    return *this;
}


void Publications::copy(const Publications &other) {
    ProjectDataItem::copy(other);

    QList<PublishedLine *> newLines;
    for(int i = 0; i < other.lineCount(); i++) {
        PublishedLine *l = other.lineAt(i);
        if(hasLine(l->id())) {
            PublishedLine *currentL = line(l->id());
            *currentL = *l;
            newLines << currentL;
        } else {
            newLines << l;
        }
    }
    setLines(newLines);
}

void Publications::setJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);

    QJsonArray jLineSchedules = jsonObject.value("lineSchedules").isArray() ? jsonObject.value("lineSchedules").toArray() : QJsonArray();

    for(int i = 0; i < jLineSchedules.count(); ++i)
        if(jLineSchedules.at(i).isObject())
            addLine(new PublishedLine(this, jLineSchedules.at(i).toObject()));
}

QJsonObject Publications::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.remove("id");

    QJsonArray jLineSchedules;

    for(int i = 0; i < lineCount(); ++i)
        jLineSchedules << lineAt(i)->toJson();

    jsonObject.insert("lineSchedules", jLineSchedules);

    return jsonObject;
}

QList<PublishedLine *> Publications::lines() const {
    return _lines;
}

int Publications::lineCount() const {
    return _lines.count();
}

PublishedLine *Publications::line(const QString &id) const {
    for(int i = 0; i < lineCount(); i++)
        if(lineAt(i)->id() == id)
            return lineAt(i);

    return nullptr;
}

PublishedLine *Publications::lineAt(const int &index) const {
    if(index < 0 || index >= lineCount())
        return nullptr;
    return _lines[index];
}

bool Publications::hasLine(const QString &id) const {
    for (int i = 0; i < lineCount(); ++i)
        if(lineAt(i)->id() ==id)
            return true;
    return false;
}

void Publications::setLines(const QList<PublishedLine *> &newLines) {
    _lines = newLines;
}

void Publications::addLine(PublishedLine *newLine) {
    if(!newLine)
        return;

    newLine->setParent(this);
    _lines << newLine;
}

void Publications::removeLine(PublishedLine *line) {
    for(int i = 0; i < lineCount(); i++)
        if(lineAt(i) == line)
            _lines.remove(i);
}

void Publications::removeLine(const QString &id) {
    for(int i = 0; i < lineCount(); i++)
        if(lineAt(i)->id() == id)
            _lines.remove(i);
}
