#include "publications.h"

Publications::Publications() {

}

QList<PublishedLine *> Publications::lines() const {
    return m_lines;
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
    return m_lines[index];
}

int Publications::lineCount() const {
    return m_lines.count();
}

void Publications::setLines(const QList<PublishedLine *> &newLines) {
    m_lines = newLines;
}

void Publications::addLine(PublishedLine *newLine) {
    if(!newLine)
        return;

    m_lines << newLine;
}

void Publications::removeLine(PublishedLine *line) {
    for(int i = 0; i < lineCount(); i++)
        if(lineAt(i) == line)
            m_lines.remove(i);
}

void Publications::removeLine(const QString &id) {
    for(int i = 0; i < lineCount(); i++)
        if(lineAt(i)->id() == id)
            m_lines.remove(i);
}
