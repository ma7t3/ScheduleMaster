#ifndef ABSTRACTPROJECTDATAITEM_H
#define ABSTRACTPROJECTDATAITEM_H

#include <QtCore>


class AbstractProjectDataItem
{

public:
    AbstractProjectDataItem(QString id) {
        this->m_id = id;
    }

    QString id() {
        return m_id;
    }

protected:
    QString m_id;
};

#endif // ABSTRACTPROJECTDATAITEM_H
