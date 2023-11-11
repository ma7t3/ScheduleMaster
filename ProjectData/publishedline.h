#ifndef PUBLISHEDLINE_H
#define PUBLISHEDLINE_H

#include <QtCore>

#include "publishedbusstop.h"
#include "ProjectData/route.h"

#include "ProjectData/publishedlinedirection.h"


class PublishedLine : public AbstractProjectDataItem {
public:
    PublishedLine(const QString &id, const QString &title = "", const QString &description = "", const QString &subTitle = "");

    QString title() const;
    void setTitle(const QString &newTitle);

    QString filePath() const;
    void setFilePath(const QString &newFilePath);

    QString subTitle() const;
    void setSubTitle(const QString &newSubTitle);

    QList<PublishedLineDirection *> directions() const;
    int directionCount() const;
    PublishedLineDirection *directionAt(const int &index);
    void setDirections(const QList<PublishedLineDirection *> &newDirections);
    void addDirection(PublishedLineDirection *direction);
    void removeDirection(PublishedLineDirection * direction);
    void removeDirection(const QString &id);

    void setNew(const PublishedLine newPublishedLine);

private:
    QString m_filePath;
    QString m_title;
    QString m_subTitle;

    QList<PublishedLineDirection *> m_directions;
};

#endif // PUBLISHEDLINE_H
