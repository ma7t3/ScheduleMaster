#include "publishedbusstop.h"

#include "ProjectData.h"

PublishedBusstop::PublishedBusstop(QObject *parent, const QString &id, Busstop *linkedBusstop, const QString &label) :
    ProjectDataItem(parent, id), _linkedBusstop(linkedBusstop), _label(label), _showDivider(false) {}

PublishedBusstop::PublishedBusstop(QObject *parent, const QJsonObject &jsonObject) :
    ProjectDataItem(parent) {
    fromJson(jsonObject);
}

PublishedBusstop::PublishedBusstop(const PublishedBusstop &other) :
    ProjectDataItem(other.parent()) {
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

void PublishedBusstop::fromJson(const QJsonObject &jsonObject) {
    ProjectDataItem::fromJson(jsonObject);

    setLinkedBusstop(static_cast<ProjectData *>(parent()->parent()->parent()->parent())->busstop(jsonObject.value("busstopID").toString("")));
    setLabel(jsonObject.value("label").toString(""));
    setShowDivider(jsonObject.value("showDivider").toBool(false));
    joinWithPrevious(jsonObject.value("joinWithPrevious").toBool(false));
    setShowArrAndDep(jsonObject.value("showArrAndDep").toBool(false));
}

QJsonObject PublishedBusstop::toJson() const {
    QJsonObject jsonObject = ProjectDataItem::toJson();
    jsonObject.insert("busstopID", linkedBusstop()->id());
    jsonObject.insert("label", label());
    jsonObject.insert("showDivider", showDivider());
    jsonObject.insert("joinWithPrevious", isJoinedWithPrevious());
    jsonObject.insert("showArrAndDep", showArrAndDep());

    return jsonObject;
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
