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

    void setNew(const PublishedBusstop &newBusstop);

private:
    Busstop *m_linkedBusstop;
    QString m_label;
    bool m_showDivider;
    bool m_joinWithPrevious;
    bool m_showArrAndDep;
};

#endif // PUBLISHEDBUSSTOP_H
