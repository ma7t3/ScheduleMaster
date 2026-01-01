#include "IAppearanceService.h"

namespace ScheduleMaster {

StyleConfig::StyleConfig(const QJsonObject &jsonObject, const int &index) : GlobalConfigItem(jsonObject, index) {
    name               = jsonObject.value("name").toString(id());
    lightSupport       = jsonObject.value("lightSupport").toBool();
    darkSupport        = jsonObject.value("darkSupport").toBool();
    accentColorSupport = jsonObject.value("accentColorSupport").toBool();
    applyPalette       = jsonObject.value("applyPalette").toBool();

    QString typeString = jsonObject.value("type").toString();
    type = typeString == "styleClass"     ? StyleClassType
           : typeString == "styleSheet"   ? StyleSheetType
           : typeString == "styleFactory" ? StyleFactoryType
                                          : InvalidType;

    switch(type) {
    case StyleClassType:   styleClassID     = jsonObject.value("styleClassID").toString();     break;
    case StyleSheetType:   styleSheetUrl    = jsonObject.value("styleSheetUrl").toString();    break;
    case StyleFactoryType: styleFactoryName = jsonObject.value("styleFactoryName").toString(); break;
    default: break;
    }
}

StyleConfig::StyleConfig(const QString &id, const int &index) : GlobalConfigItem(id, index) {}

bool StyleConfig::supportsColorScheme(const Qt::ColorScheme &colorScheme) const {
    return (
        (colorScheme == Qt::ColorScheme::Unknown && lightSupport && darkSupport) ||
        (colorScheme == Qt::ColorScheme::Light && lightSupport) ||
        (colorScheme == Qt::ColorScheme::Dark && darkSupport)
        );
}

QList<Qt::ColorScheme> StyleConfig::supportedColorSchemes() const {
    if(supportsColorScheme(Qt::ColorScheme::Unknown))
        return {Qt::ColorScheme::Unknown, Qt::ColorScheme::Light, Qt::ColorScheme::Dark};

    if(supportsColorScheme(Qt::ColorScheme::Light))
        return {Qt::ColorScheme::Light};

    if(supportsColorScheme(Qt::ColorScheme::Dark))
        return {Qt::ColorScheme::Dark};

    return {};
}

} // namespace ScheduleMaster
