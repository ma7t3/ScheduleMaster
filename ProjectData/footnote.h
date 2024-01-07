#ifndef FOOTNOTE_H
#define FOOTNOTE_H

#include <QObject>

#include "ProjectDataItem.h"

// ABGESCHLOSSEN!

class Footnote : public virtual ProjectDataItem {
    Q_OBJECT
public:
    Footnote(const QString &id, const QString &identifier, const QString &description = "");
    Footnote(const Footnote &);
    Footnote operator=(const Footnote &);

    QString identifier() const;
    void setIdentifier(const QString &newIdentifier);

    QString description() const;
    void setDescription(const QString &newDescription);

protected:
    void copy(const Footnote &);

private:
    QString _identifier, _description;
};

#endif // FOOTNOTE_H
