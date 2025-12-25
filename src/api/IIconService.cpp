#include "IIconService.h"

namespace ScheduleMaster {

IconSetConfig::IconSetConfig(const QJsonObject &jsonObject, const int &index) : GlobalConfigItem(jsonObject, index) {
    name = jsonObject.value("name").toString();
    alternative = jsonObject.value("alternative").toString();
    format = jsonObject.value("format").toString();
}

IconSetConfig::IconSetConfig(const QString &id, const int &index) : GlobalConfigItem(id, index) {}

}
