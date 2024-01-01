#ifndef BUSSTOP_H
#define BUSSTOP_H

#pragma once

#include <QtCore>
#include "abstractprojectdataitem.h"

class Busstop : public AbstractProjectDataItem
{
private:
    QString _name;
    bool _important;

public:
    Busstop(QString id, QString name, bool important);
    QString name();
    bool isImportant();
    bool important();

    void setName(QString);
    void setImportant(bool);

    void overwrite(Busstop &other);
    bool operator<(Busstop &other);
};

#endif // BUSSTOP_H
