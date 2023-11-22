#include "publishedbusstop.h"

PublishedBusstop::PublishedBusstop(const QString &id, Busstop *linkedBusstop, const QString &label) :
    AbstractProjectDataItem(id),
    m_linkedBusstop(linkedBusstop),
    m_label(label) {
}

Busstop *PublishedBusstop::linkedBusstop() const {
    return m_linkedBusstop;
}

void PublishedBusstop::setLinkedBusstop(Busstop *newLinkedBusstop) {
    m_linkedBusstop = newLinkedBusstop;
}

QString PublishedBusstop::label() const {
    return m_label;
}

void PublishedBusstop::setLabel(const QString &newLabel) {
    m_label = newLabel;
}

void PublishedBusstop::removeCustomLabel() {
    setLabel("");
}

bool PublishedBusstop::isJoinedWithPrevious() const {
    return m_joinWithPrevious;
}

void PublishedBusstop::joinWithPrevious(bool b) {
    m_joinWithPrevious = b;
}

bool PublishedBusstop::showArrAndDep() const {
    return m_showArrAndDep;
}

void PublishedBusstop::setShowArrAndDep(bool newShowArrAndDep) {
    m_showArrAndDep = newShowArrAndDep;
}

bool PublishedBusstop::showDivider() const {
    return m_showDivider;
}

void PublishedBusstop::setShowDivider(bool newShowDivider) {
    m_showDivider = newShowDivider;
}

void PublishedBusstop::setNew(const PublishedBusstop &newBusstop) {
    setLinkedBusstop(newBusstop.linkedBusstop());
    setLabel(newBusstop.label());
    setShowDivider(newBusstop.showDivider());
    joinWithPrevious(newBusstop.isJoinedWithPrevious());
    setShowArrAndDep(newBusstop.showArrAndDep());
}



















