#ifndef FOOTNOTE_H
#define FOOTNOTE_H

#include "ProjectData/abstractprojectdataitem.h"

#include <QObject>

class Footnote : public AbstractProjectDataItem {
    //Q_OBJECT
public:
    Footnote(QString id, QString identifier = "", QString description = "");

    QString identifier() const;
    void setIdentifier(const QString &newIdentifier);

    QString description() const;
    void setDescription(const QString &newDescription);

    void overwrite(Footnote);

private:
    QString _identifier, _description;
};

#endif // FOOTNOTE_H
