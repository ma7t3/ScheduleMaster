#ifndef PROJECTDATAITEM_H
#define PROJECTDATAITEM_H

#include <QtCore>


class ProjectDataItem : public QObject {
public:
    ProjectDataItem(const QString &id = "");
    ProjectDataItem(const QJsonObject &);
    ProjectDataItem(const ProjectDataItem &);
    ProjectDataItem operator=(const ProjectDataItem &);

    QString id() const;

    ProjectDataItem fromJson(const QJsonObject &);
    QJsonObject toJson() const;

protected:
    void copy(const ProjectDataItem &);

    QString _id;
};

#endif // PROJECTDATAITEM_H
