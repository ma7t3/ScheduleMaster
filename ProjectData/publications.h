#ifndef PUBLICATIONS_H
#define PUBLICATIONS_H

#include <QtCore>
#include "publishedline.h"

// ABGESCHLOSSEN!

class Publications : public virtual ProjectDataItem {
    Q_OBJECT
public:
    Publications();
    Publications(const Publications &);
    Publications operator=(const Publications &);

    QList<PublishedLine *> lines() const;
    int lineCount() const;
    PublishedLine *line(const QString &id) const;
    PublishedLine *lineAt(const int &index) const;
    bool hasLine(const QString &id) const;

    void setLines(const QList<PublishedLine *> &newLines);
    void addLine(PublishedLine *newLine);
    void removeLine(PublishedLine *line);
    void removeLine(const QString &id);

protected:
    void copy(const Publications &);

private:
    QList<PublishedLine *> _lines;
};

#endif // PUBLICATIONS_H
