#ifndef PROJECTDATAITEM_H
#define PROJECTDATAITEM_H

#include <QtCore>

class ProjectDataItem : public QObject {
    Q_OBJECT
public:
    ProjectDataItem(QObject *parent, const QString &id = "");
    ProjectDataItem(QObject *parent, const QJsonObject &);
    ProjectDataItem(const ProjectDataItem &);
    ProjectDataItem operator=(const ProjectDataItem &);

    QString id() const;

    bool inUse() const;
    void setInUse(const bool &newInUse);

    QJsonObject toJson() const;

    static QString getNewID();

signals:
    void changed();

protected:
    void copy(const ProjectDataItem &);
    void fromJson(const QJsonObject &);

    QString _id;

    bool _inUse;
};

#endif // PROJECTDATAITEM_H
