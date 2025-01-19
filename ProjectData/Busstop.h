#ifndef BUSSTOP_H
#define BUSSTOP_H

#include <QObject>

#include "ProjectDataItem.h"

class Busstop : public ProjectDataItem {
    Q_OBJECT
public:
    Busstop(QObject *parent, const QUuid &id = QUuid());
    Busstop(QObject *parent, const QJsonObject &);

    bool operator<(const Busstop &);

    struct Data {
        QString name;
        bool important = false;
    };

    Data data() const;
    void setData(const Data &);

    QString name() const;
    void setName(const QString &);

    bool important() const;
    void setImportant(const bool &);

    QJsonObject toJson() const;

protected:
    void fromJson(const QJsonObject &);

signals:
    void changed();

private:
    Busstop::Data _data;
};

#endif // BUSSTOP_H
