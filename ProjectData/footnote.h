#ifndef FOOTNOTE_H
#define FOOTNOTE_H

#include <QObject>

#include "ProjectDataItem.h"

class Footnote : public virtual ProjectDataItem {
    Q_OBJECT
public:
    Footnote(QObject *parent, const QString &id, const QString &identifier, const QString &name, const QString &description = "");
    Footnote(const Footnote &);
    Footnote operator=(const Footnote &);
    bool operator<(const Footnote &);

    QString identifier() const;
    void setIdentifier(const QString &newIdentifier);

    QString name() const;
    void setName(const QString &newName);

    QString description() const;
    void setDescription(const QString &newDescription);


protected:
    void copy(const Footnote &);

private:
    QString _identifier, _name, _description;
};

#endif // FOOTNOTE_H
