#include "LanguageManager.h"

#include "SettingsManager.h"

LanguageConfig::LanguageConfig(const QString &id) : GlobalConfigItem(id) {}

LanguageConfig::LanguageConfig(const QJsonObject &json)
    : GlobalConfigItem(json), locale(id()) {}

LanguageManager::LanguageManager(QObject *parent) : GlobalConfigManager(parent) {
    loadItems("Languages");
    QString str = SettingsManager::value("general.language").toString();
    QLocale tmpLocale(str);
    QLocale locale;
    if(str == "system" || !LanguageManager::isSupported(tmpLocale.name()))
        locale = QLocale::system();
    else
        locale = QLocale(str);

    QLocale::setDefault(locale);
}

QList<LanguageConfig> LanguageManager::supportedLanguages() {
    return items();
}

bool LanguageManager::isSupported(const QString &languageID) {
    return itemExists(languageID);
}

QLocale LanguageManager::currentLocale() {
    return QLocale(SettingsManager::value("general.language").toString());
}

QLocale::Language LanguageManager::currentLanguage() {
    return currentLocale().language();
}

void LanguageManager::setCurrentLanguage(const QString &languageID) {
    SettingsManager::setValue("general.language", languageID);
    emit instance()->currentLanguageChanged(languageID);
}
