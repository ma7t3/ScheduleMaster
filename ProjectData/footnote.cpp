#include "footnote.h"

Footnote::Footnote(QObject *parent, const QString &id, const QString &identifier, const QString &description) :
    ProjectDataItem(parent, id),
    _identifier(identifier),
    _description(description) {
}

Footnote::Footnote(const Footnote &other) :
    ProjectDataItem(other.parent()) {
    copy(other);
}

Footnote Footnote::operator=(const Footnote &other) {
    copy(other);
    return *this;
}

void Footnote::copy(const Footnote &other) {
    ProjectDataItem::copy(other);
    setIdentifier(other.identifier());
    setDescription(other.description());
}

QString Footnote::identifier() const {
    return _identifier;
}

void Footnote::setIdentifier(const QString &newIdentifier) {
    _identifier = newIdentifier;
}

QString Footnote::description() const {
    return _description;
}

void Footnote::setDescription(const QString &newDescription) {
    _description = newDescription;
}


