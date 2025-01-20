#ifndef PROJECTDATAITEM_H
#define PROJECTDATAITEM_H

#include <QObject>
#include <QJsonObject>

class ProjectDataItem : public QObject {
    Q_OBJECT
public:
    ProjectDataItem(QObject *parent, const QUuid &id = QUuid());

    QUuid id() const;
    QString idAsString() const;

    static QUuid generateID();

    QJsonObject toJson() const;

signals:

protected:
    void fromJson(const QJsonObject &);

private:
    QUuid _id;
};

#endif // PROJECTDATAITEM_H
