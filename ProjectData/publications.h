#ifndef PUBLICATIONS_H
#define PUBLICATIONS_H

#include <QtCore>

#include "publishedline.h"


class Publications
{
public:
    Publications();

    QList<PublishedLine *> lines() const;
    PublishedLine *line(const QString &id) const;
    PublishedLine *lineAt(const int &index) const;
    int lineCount() const;

    void setLines(const QList<PublishedLine *> &newLines);
    void addLine(PublishedLine *newLine);
    void removeLine(PublishedLine *line);
    void removeLine(const QString &id);

private:
    QList<PublishedLine *> _lines;
};

#endif // PUBLICATIONS_H
