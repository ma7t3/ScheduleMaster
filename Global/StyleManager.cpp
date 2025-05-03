#include "StyleManager.h"

Style::Style(const QJsonObject &jsonObject) : GlobalConfigItem(jsonObject) {
    name               = jsonObject.value("name").toString(id());
    lightSupport       = jsonObject.value("lightSupport").toBool();
    darkSupport        = jsonObject.value("darkSupport").toBool();
    accentColorSupport = jsonObject.value("accentColorSupport").toBool();
    applyPalette       = jsonObject.value("applyPalette").toBool();

    QString typeString = jsonObject.value("type").toString();
    type = typeString == "systemDefault" ? SystemDefaultType
           : typeString == "styleClass"    ? StyleClassType
           : typeString == "styleSheet"    ? StyleSheetType
           : typeString == "styleFactory"  ? StyleFactoryType
                                          : InvalidType;

    switch(type) {
    case StyleClassType:   styleClassID     = jsonObject.value("styleClassID").toString();     break;
    case StyleSheetType:   styleSheetUrl    = jsonObject.value("styleSheetUrl").toString();    break;
    case StyleFactoryType: styleFactoryName = jsonObject.value("styleFactoryName").toString(); break;
    default: break;
    }
}

Style::Style(const QString &id) : GlobalConfigItem(id) {}

bool Style::supportsColorScheme(const Qt::ColorScheme &colorScheme) const {
    return (
        (colorScheme == Qt::ColorScheme::Unknown && lightSupport && darkSupport) ||
        (colorScheme == Qt::ColorScheme::Light && lightSupport) ||
        (colorScheme == Qt::ColorScheme::Dark && darkSupport)
        );
}

QList<Qt::ColorScheme> Style::supportedColorSchemes() const {
    if(supportsColorScheme(Qt::ColorScheme::Unknown))
        return {Qt::ColorScheme::Unknown, Qt::ColorScheme::Light, Qt::ColorScheme::Dark};

    if(supportsColorScheme(Qt::ColorScheme::Light))
        return {Qt::ColorScheme::Light};

    if(supportsColorScheme(Qt::ColorScheme::Dark))
        return {Qt::ColorScheme::Dark};

    return {};
}

StyleManager::StyleManager(QObject *parent) : GlobalConfigManager(parent) {
    loadItems("Styles");
    loadAccentColors();
}

QColor StyleManager::accentColor(const QString &id) {
    return _accentColors.value(id, QColor());
}

QColor StyleManager::accentColorDark(const QString &id) {
    QColor color = accentColor(id);
    return QColor(color.red() * 0.5, color.green() * 0.5, color.blue() * 0.5);
}

QMap<QString, QColor> StyleManager::accentColors() {
    return _accentColors;
}

void StyleManager::loadAccentColors() {
    const QJsonDocument doc = loadSingleConfigResource("accentColors");
    const QJsonObject object = doc.object();
    const QStringList keys = object.keys();
    for(const QString &key : keys)
        _accentColors.insert(key, QColor(object[key].toString()));
}
