#ifndef BUSSTOP_H
#define BUSSTOP_H

#include <QtCore>

#include "ProjectDataItem.h"

// ABGESCHLOSSEN!

class Busstop : public virtual ProjectDataItem {
    Q_OBJECT
public:
    Busstop(const QString &id, const QString &name, const bool &important = false);
    Busstop(const Busstop &);
    Busstop operator=(const Busstop &other);
    bool operator<(const Busstop &other);

    void setName(const QString &);
    QString name() const;

    void setImportant(const bool &);
    bool isImportant() const;
    bool important() const;

protected:
    void copy(const Busstop &other);

private:
    QString _name;
    bool _important;
};

#endif // BUSSTOP_H
