#include "StyleManager.h"

#include <QGuiApplication>
#include <QFont>

#include "SettingsManager.h"

StyleConfig::StyleConfig(const QJsonObject &jsonObject) : GlobalConfigItem(jsonObject) {
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

StyleConfig::StyleConfig(const QString &id) : GlobalConfigItem(id) {}

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

QString StyleManager::currentStyle() {
    return SettingsManager::value("appearance.theme").toString();
}

void StyleManager::setCurrentStyle(const QString &styleID) {
    if(!itemExists(styleID))
        return;

    SettingsManager::setValue("appearance.theme", styleID);
    emit instance()->styleChanged(styleID);
}

Qt::ColorScheme StyleManager::currentColorScheme() {
    int intVal = SettingsManager::value("appearance.colorScheme").toInt();
    if(intVal < 0 || intVal > 2)
        return Qt::ColorScheme::Unknown;

    return static_cast<Qt::ColorScheme>(intVal);
}

void StyleManager::setCurrentColorScheme(const Qt::ColorScheme &colorScheme) {
    SettingsManager::setValue("appearance.colorScheme", static_cast<int>(colorScheme));
    emit instance()->colorSchemeChanged(colorScheme);
}

QString StyleManager::currentAccentColorID() {
    return SettingsManager::value("appearance.accentColor").toString();
}

QColor StyleManager::currentAccentColor() {
    return accentColor(currentAccentColorID());
}

void StyleManager::setCurrentAccentColor(const QString &accentColorID) {
    SettingsManager::setValue("appearance.accentColor", accentColorID);
    emit instance()->accentColorChanged(accentColorID);
}

QString StyleManager::currentUiFontFamily() {
    QString str = SettingsManager::value("appearance.font").toString();
    if(str.isEmpty())
        return qGuiApp->font().family();
    else
        return str;
}

void StyleManager::setCurrentUiFontFamily(const QString &fontFamily) {
    SettingsManager::setValue("appearance.font", fontFamily);
    emit instance()->uiFontFamilyChanged(fontFamily);
}

bool StyleManager::gdiFontEngineEnabled() {
    return SettingsManager::value("appearance.fontEngineGDI").toBool();
}

void StyleManager::setGdiFontEngineEnabled(const bool &enabled) {
    SettingsManager::setValue("appearance.fontEngineGDI", enabled);
}

void StyleManager::loadAccentColors() {
    const QJsonDocument doc = loadSingleConfigResource("accentColors");
    const QJsonObject object = doc.object();
    const QStringList keys = object.keys();
    for(const QString &key : keys)
        _accentColors.insert(key, QColor(object[key].toString()));
}
