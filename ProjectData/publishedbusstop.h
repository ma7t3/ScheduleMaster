#ifndef PUBLISHEDBUSSTOP_H
#define PUBLISHEDBUSSTOP_H

#include <QtCore>

#include "busstop.h"

class PublishedBusstop : public virtual ProjectDataItem {
    Q_OBJECT
public:
    PublishedBusstop(QObject *parent, const QString &id);
    PublishedBusstop(QObject *parent, const QJsonObject &);
    PublishedBusstop(const PublishedBusstop &);
    PublishedBusstop operator=(const PublishedBusstop &);

    Busstop *linkedBusstop() const;
    void setLinkedBusstop(Busstop *newLinkedBusstop);

    QString label() const;
    void setLabel(const QString &newLabel);
    void removeCustomLabel();

    bool isJoinedWithPrevious() const;
    void joinWithPrevious(bool b);

    bool showArrAndDep() const;
    void setShowArrAndDep(bool newShowArrAndDep);

    bool showDivider() const;
    void setShowDivider(bool newShowDivider);

    QJsonObject toJson() const;

protected:
    void copy(const PublishedBusstop &);
    void fromJson(const QJsonObject &);

private:
    Busstop *_linkedBusstop;
    QString _label;
    bool _showDivider;
    bool _joinWithPrevious;
    bool _showArrAndDep;
};

#endif // PUBLISHEDBUSSTOP_H
