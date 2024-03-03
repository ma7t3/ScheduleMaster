#ifndef PROJECTDATAITEM_H
#define PROJECTDATAITEM_H

#include <QtCore>

class ProjectDataItem : public QObject {
public:
    ProjectDataItem(QObject *parent, const QString &id = "");
    ProjectDataItem(QObject *parent, const QJsonObject &);
    ProjectDataItem(const ProjectDataItem &);
    ProjectDataItem operator=(const ProjectDataItem &);

    QString id() const;

    QJsonObject toJson() const;

protected:
    void copy(const ProjectDataItem &);
    void fromJson(const QJsonObject &);

    QString _id;
};

#endif // PROJECTDATAITEM_H
