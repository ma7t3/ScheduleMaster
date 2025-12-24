#include "LanguageManager.h"

#include "src/namespace.h"
#include "src/core/SettingsServiceImpl.h"

LanguageConfig::LanguageConfig(const QString &id, const int &index) : GlobalConfigItem(id, index) {}

LanguageConfig::LanguageConfig(const QJsonObject &json, const int &index) : GlobalConfigItem(json, index), locale(id()) {}

LanguageManager::LanguageManager(QObject *parent) : GlobalConfigManager(parent) {
    loadItems("Languages");
    QString str = SM::SettingsServiceImpl::instance()->value("general.language").toString();
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
    return QLocale(SM::SettingsServiceImpl::instance()->value("general.language").toString());
}

QLocale::Language LanguageManager::currentLanguage() {
    return currentLocale().language();
}

void LanguageManager::setCurrentLanguage(const QString &languageID) {
    SM::SettingsServiceImpl::instance()->setValue("general.language", languageID);
    emit instance()->currentLanguageChanged(languageID);
}
