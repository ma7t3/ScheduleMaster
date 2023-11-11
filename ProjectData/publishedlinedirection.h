#ifndef PUBLISHEDLINEDIRECTION_H
#define PUBLISHEDLINEDIRECTION_H


#include <QtCore>

#include "publishedbusstop.h"
#include "ProjectData/route.h"

class PublishedLineDirection : public AbstractProjectDataItem {
public:
    PublishedLineDirection(const QString &id, const QString &name);

    QList<PublishedBusstop *> busstops() const;
    void setBusstops(const QList<PublishedBusstop *> &newBusstops);

    QList<Route *> routes() const;
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
