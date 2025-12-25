#include "ILanguageService.h"

namespace ScheduleMaster {

LanguageConfig::LanguageConfig(const QString &id, const int &index) : GlobalConfigItem(id, index) {}

LanguageConfig::LanguageConfig(const QJsonObject &json, const int &index) :
    GlobalConfigItem(json, index), locale(id()) {}

} // namespace ScheduleMaster
