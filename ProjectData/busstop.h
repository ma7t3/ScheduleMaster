#ifndef BUSSTOP_H
#define BUSSTOP_H

#pragma once

#include <QtCore>
#include "abstractprojectdataitem.h"

class Busstop : public AbstractProjectDataItem
{
private:
    QString m_name;
    bool m_important;

public:
    Busstop(QString id, QString name, bool important);
    QString name();
    bool isImportant();
    bool important();

    void setName(QString);
    void setImportant(bool);
};

#endif // BUSSTOP_H
