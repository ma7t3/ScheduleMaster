#include "LanguageManager.h"

LanguageConfig::LanguageConfig(const QString &id) : GlobalConfigItem(id) {}

LanguageConfig::LanguageConfig(const QJsonObject &json)
    : GlobalConfigItem(json), locale(id()) {}

LanguageManager::LanguageManager(QObject *parent) : GlobalConfigManager(parent) {
    loadItems("Languages");
}

QList<LanguageConfig> LanguageManager::supportedLanguages() {
    return items();
}

bool LanguageManager::isSupported(const QString &languageID) {
    return itemExists(languageID);
}
