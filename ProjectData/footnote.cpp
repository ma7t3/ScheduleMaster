#include "footnote.h"

Footnote::Footnote(QString id, QString identifier, QString description) :
    AbstractProjectDataItem(id),
    _identifier(identifier),
    _description(description) {
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

void Footnote::overwrite(Footnote newF) {
    setIdentifier(newF.identifier());
    setDescription(newF.description());
}
