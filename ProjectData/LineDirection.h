#ifndef LINEDIRECTION_H
#define LINEDIRECTION_H

#include "ProjectDataItem.h"

class LineDirection : public ProjectDataItem {
    Q_OBJECT
public:
    LineDirection(QObject *parent, const QUuid &id = QUuid());
    LineDirection(QObject *parent, const QJsonObject &);

    struct Data {
        QString description;
    };

    Data data() const;
    void setData(const Data &);

    QString description() const;
    void setDescription(const QString &);

    QJsonObject toJson() const;

protected:
    void fromJson(const QJsonObject &);

signals:
    void changed();

private:
    LineDirection::Data _data;
};

#endif // LINEDIRECTION_H
