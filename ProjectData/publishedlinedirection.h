#ifndef PUBLISHEDLINEDIRECTION_H
#define PUBLISHEDLINEDIRECTION_H


#include <QtCore>

#include "publishedbusstop.h"
#include "ProjectData/route.h"

class PublishedLineDirection : public AbstractProjectDataItem {
public:
    PublishedLineDirection(const QString &id, const QString &name);

    QList<PublishedBusstop *> busstops() const;
    PublishedBusstop *busstop(const QString &id);
    PublishedBusstop *busstopAt(const int &index);
    int busstopCount() const;
    void setBusstops(const QList<PublishedBusstop *> &newBusstops);

    QList<Route *> routes() const;
    Route *routeAt(const int &index);
    int routeCount();
    bool hasRoute(Route *);

    void setRoutes(const QList<Route *> &newRoutes);

    QString name() const;
    void setName(const QString &newName);

    void setNew(const PublishedLineDirection &newDirection);

private:
    QString m_name;
    QList<PublishedBusstop *> m_busstops;
    QList<Route *> m_routes;
};

#endif // PUBLISHEDLINEDIRECTION_H
