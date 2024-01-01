#ifndef ABSTRACTPROJECTDATAITEM_H
#define ABSTRACTPROJECTDATAITEM_H

#include <QtCore>


class AbstractProjectDataItem
{

public:
    AbstractProjectDataItem(QString id) {
        this->_id = id;
    }

    QString id() {
        return _id;
    }

protected:
    QString _id;
};

#endif // ABSTRACTPROJECTDATAITEM_H
