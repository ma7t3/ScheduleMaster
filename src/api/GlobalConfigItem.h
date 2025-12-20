#ifndef GLOBALCONFIGITEM_H
#define GLOBALCONFIGITEM_H

#include <QString>
#include <QJsonObject>

namespace ScheduleMaster {

struct GlobalConfigItem {
public:
    GlobalConfigItem(const QString &id, const int &index = 0);
    GlobalConfigItem(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);

    bool operator==(const GlobalConfigItem &other) const;
    QString id() const;
    int index() const;

private:
    QString _id;
    int _index;
};

}

#endif // GLOBALCONFIGITEM_H
