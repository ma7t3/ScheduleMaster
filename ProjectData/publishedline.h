#ifndef PUBLISHEDLINE_H
#define PUBLISHEDLINE_H

#include <QtCore>

#include "daytype.h"
#include "publishedlinedirection.h"

class PublishedLine : public virtual ProjectDataItem {
    Q_OBJECT
public:
    PublishedLine(QObject *parent, const QString &id);
    PublishedLine(QObject *parent, const QJsonObject &);
    PublishedLine(const PublishedLine &);
    PublishedLine operator=(const PublishedLine &);
    bool operator<(const PublishedLine &);

    QString title() const;
    void setTitle(const QString &newTitle);

    QString filePath() const;
    void setFilePath(const QString &newFilePath);

    QString footer() const;
    void setFooter(const QString &newSubTitle);

    int hourBreak() const;
    void setHourBreak(const int &newHourBreak);

    QList<PublishedLineDirection *> directions() const;
    int directionCount() const;
    PublishedLineDirection *direction(const QString &id) const;
    PublishedLineDirection *directionAt(const int &index) const;
    bool hasDirection(const QString &id) const;

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

    QJsonObject toJson() const;

    void refreshChilds();

    PublishedLineDirection *newDirection(QString id = "");
    PublishedLineDirection *newDirection(const QJsonObject &);
    PublishedLineDirection *newDirection(const PublishedLineDirection &newDirection);

protected:
    void copy(const PublishedLine &);
    void fromJson(const QJsonObject &);

private:
    QString _filePath;
    QString _title;
    QString _footer;
    int _hourBreak;

    QList<PublishedLineDirection *> _directions;
    QList<DayType *> _dayTypes;
};

#endif // PUBLISHEDLINE_H
