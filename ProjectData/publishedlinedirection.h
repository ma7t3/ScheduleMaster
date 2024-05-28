#ifndef PUBLISHEDLINEDIRECTION_H
#define PUBLISHEDLINEDIRECTION_H


#include <QtCore>

#include "publishedbusstop.h"
#include "route.h"

class PublishedLineDirection : public virtual ProjectDataItem {
    Q_OBJECT
public:
    PublishedLineDirection(QObject *parent, const QString &id);
    PublishedLineDirection(QObject *parent, const QJsonObject &);
    PublishedLineDirection(const PublishedLineDirection &);
    PublishedLineDirection operator=(const PublishedLineDirection &);


    QList<PublishedBusstop *> busstops() const;
    int busstopCount() const;
    PublishedBusstop *busstop(const QString &id) const;
    PublishedBusstop *busstopAt(const int &index) const;
    bool hasBusstop(const QString &id) const;

    void setBusstops(const QList<PublishedBusstop *> &newBusstops);
    void addBusstop(PublishedBusstop *newBusstop);
    void addBusstopAt(const int &index, PublishedBusstop *newBusstop);
    void removeBusstopAt(const int &index);
    void removeBusstop(const QString &id);
    void removeBusstop(PublishedBusstop *b);

    QList<Route *> routes() const;
    Route *routeAt(const int &index) const;
    int routeCount() const;
    bool hasRoute(Route *);

    void setRoutes(const QList<Route *> &newRoutes);
    void addRoute(Route *newRoute);

    QString name() const;
    void setName(const QString &newName);

    QJsonObject toJson() const;

    PublishedBusstop *newBusstop(QString id = "");
    PublishedBusstop *newBusstop(const QJsonObject &);
    PublishedBusstop *newBusstop(const PublishedBusstop &newBusstop);

protected:
    void copy(const PublishedLineDirection &);
    void fromJson(const QJsonObject &);

    //void ovoverwrite(const PublishedLineDirection &other);

private:
    QString _name;
    QList<PublishedBusstop *> _busstops;
    QList<Route *> _routes;
};

#endif // PUBLISHEDLINEDIRECTION_H
