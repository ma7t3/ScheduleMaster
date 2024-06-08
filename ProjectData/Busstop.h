#ifndef BUSSTOP_H
#define BUSSTOP_H

#include <QtCore>

#include "ProjectDataItem.h"

class Busstop : public ProjectDataItem {
    Q_OBJECT
public:
    Busstop(QObject *parent, const QString &id);
    Busstop(QObject *parent, const QJsonObject &);
    Busstop(const Busstop &);
    Busstop operator=(const Busstop &other);
    bool operator<(const Busstop &other);

    void setName(const QString &);
    QString name() const;

    void setImportant(const bool &);
    bool isImportant() const;
    bool important() const;

    QJsonObject toJson() const;

signals:
    void changed(Busstop *);

protected:
    void copy(const Busstop &other);
    void fromJson(const QJsonObject &);

private:
    QString _name;
    bool _important;
};

#endif // BUSSTOP_H
