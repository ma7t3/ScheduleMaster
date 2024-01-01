#include "publications.h"

Publications::Publications() {

}

QList<PublishedLine *> Publications::lines() const {
    return _lines;
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

int Publications::lineCount() const {
    return _lines.count();
}

void Publications::setLines(const QList<PublishedLine *> &newLines) {
    _lines = newLines;
}

void Publications::addLine(PublishedLine *newLine) {
    if(!newLine)
        return;

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
