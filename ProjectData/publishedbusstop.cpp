#include "publishedbusstop.h"

PublishedBusstop::PublishedBusstop(const QString &id, Busstop *linkedBusstop, const QString &label) :
    AbstractProjectDataItem(id),
    _linkedBusstop(linkedBusstop),
    _label(label),
    _showDivider(false) {
}

Busstop *PublishedBusstop::linkedBusstop() const {
    return _linkedBusstop;
}

void PublishedBusstop::setLinkedBusstop(Busstop *newLinkedBusstop) {
    _linkedBusstop = newLinkedBusstop;
}

QString PublishedBusstop::label() const {
    return _label;
}

void PublishedBusstop::setLabel(const QString &newLabel) {
    _label = newLabel;
}

void PublishedBusstop::removeCustomLabel() {
    setLabel("");
}

bool PublishedBusstop::isJoinedWithPrevious() const {
    return _joinWithPrevious;
}

void PublishedBusstop::joinWithPrevious(bool b) {
    _joinWithPrevious = b;
}

bool PublishedBusstop::showArrAndDep() const {
    return _showArrAndDep;
}

void PublishedBusstop::setShowArrAndDep(bool newShowArrAndDep) {
    _showArrAndDep = newShowArrAndDep;
}

bool PublishedBusstop::showDivider() const {
    return _showDivider;
}

void PublishedBusstop::setShowDivider(bool newShowDivider) {
    _showDivider = newShowDivider;
}

void PublishedBusstop::overwrite(const PublishedBusstop &newBusstop) {
    setLinkedBusstop(newBusstop.linkedBusstop());
    setLabel(newBusstop.label());
    setShowDivider(newBusstop.showDivider());
    joinWithPrevious(newBusstop.isJoinedWithPrevious());
    setShowArrAndDep(newBusstop.showArrAndDep());
}



















