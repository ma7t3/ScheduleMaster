#include "GlobalConfigItem.h"

namespace ScheduleMaster {

GlobalConfigItem::GlobalConfigItem(const QString &id, const int &index) : _id(id), _index(index) {}

GlobalConfigItem::GlobalConfigItem(const QJsonObject &jsonObject, const int &index) :
    _index(index) {
    _id = jsonObject.value("id").toString();
}

bool GlobalConfigItem::operator==(const GlobalConfigItem &other) const {
    return id() == other.id();
}

QString GlobalConfigItem::id() const {
    return _id;
}

int GlobalConfigItem::index() const {
    return _index;
}

}
