#ifndef PUBLICATIONS_H
#define PUBLICATIONS_H

#include <QtCore>
#include "publishedline.h"

class Publications : public virtual ProjectDataItem {
    Q_OBJECT
public:
    Publications(QObject *parent);
    Publications(QObject *parent, const QJsonObject &);
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

    QJsonObject toJson() const;
    void setJson(const QJsonObject &);

    void refreshChilds();

protected:
    void copy(const Publications &);

private:
    QList<PublishedLine *> _lines;
};

#endif // PUBLICATIONS_H
