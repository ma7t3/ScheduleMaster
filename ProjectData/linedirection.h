#ifndef LINEDIRECTION_H
#define LINEDIRECTION_H

#include "ProjectData/abstractprojectdataitem.h"

class LineDirection : public AbstractProjectDataItem
{
public:
    LineDirection(QString id, QString description = "");

    void setDescription(QString);
    QString description();

    LineDirection *operator=(LineDirection &other);

private:
    QString m_description;
};

#endif // LINEDIRECTION_H
