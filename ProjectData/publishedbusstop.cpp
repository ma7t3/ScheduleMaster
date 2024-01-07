#include "publishedbusstop.h"

PublishedBusstop::PublishedBusstop(const QString &id, Busstop *linkedBusstop, const QString &label) :
    ProjectDataItem(id), _linkedBusstop(linkedBusstop), _label(label), _showDivider(false) {}

PublishedBusstop::PublishedBusstop(const PublishedBusstop &other) {
    copy(other);
}

PublishedBusstop PublishedBusstop::operator=(const PublishedBusstop &other) {
    copy(other);
    return *this;
}

void PublishedBusstop::copy(const PublishedBusstop &other) {
    ProjectDataItem::copy(other);
    setLinkedBusstop(other.linkedBusstop());
    setLabel(other.label());
    setShowDivider(other.showDivider());
    joinWithPrevious(other.isJoinedWithPrevious());
    setShowArrAndDep(other.showArrAndDep());
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
