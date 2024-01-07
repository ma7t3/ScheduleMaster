#ifndef PUBLISHEDLINE_H
#define PUBLISHEDLINE_H

#include <QtCore>

#include "daytype.h"
#include "publishedlinedirection.h"

// ABGESCHLOSSEN!

class PublishedLine : public virtual ProjectDataItem {
    Q_OBJECT
public:
    PublishedLine(const QString &id, const QString &title, const QString &footer = "");
    PublishedLine(const PublishedLine &);
    PublishedLine operator=(const PublishedLine &);

    QString title() const;
    void setTitle(const QString &newTitle);

    QString filePath() const;
    void setFilePath(const QString &newFilePath);

    QString footer() const;
    void setFooter(const QString &newSubTitle);

    QList<PublishedLineDirection *> directions() const;
    int directionCount() const;
    PublishedLineDirection *direction(const QString &id);
    PublishedLineDirection *directionAt(const int &index);
    void setDirections(const QList<PublishedLineDirection *> &newDirections);
    void addDirection(PublishedLineDirection *direction);
    void removeDirection(PublishedLineDirection * direction);
    void removeDirection(const QString &id);

    QList<DayType *> dayTypes() const;
    int dayTypeCount() const;
    bool hasDayType(DayType *);

    void setDayTypes(const QList<DayType *> &newDayTypes);
    void addDayType(DayType *);
    void removeDayType(DayType *);

protected:
    void copy(const PublishedLine &);

private:
    QString _filePath;
    QString _title;
    QString _footer;

    QList<PublishedLineDirection *> _directions;
    QList<DayType *> _dayTypes;
};

#endif // PUBLISHEDLINE_H
