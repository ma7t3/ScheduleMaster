#ifndef PUBLISHEDBUSSTOP_H
#define PUBLISHEDBUSSTOP_H

#include "ProjectData/busstop.h"
#include <QtCore>

class PublishedBusstop : public AbstractProjectDataItem
{
public:
    PublishedBusstop(const QString &id, Busstop *linkedBusstop, const QString &label = "");

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

    void overwrite(const PublishedBusstop &other);

private:
    Busstop *_linkedBusstop;
    QString _label;
    bool _showDivider;
    bool _joinWithPrevious;
    bool _showArrAndDep;
};

#endif // PUBLISHEDBUSSTOP_H
