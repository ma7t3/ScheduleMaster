#include "LanguagesManager.h"

Language::Language(const QString &id) : GlobalConfigItem(id) {}

Language::Language(const QJsonObject &json)
    : GlobalConfigItem(json), locale(id()) {}

LanguagesManager::LanguagesManager(QObject *parent) : GlobalConfigManager(parent) {
    loadItems("Languages");
}

QList<Language> LanguagesManager::supportedLanguages() {
    return items();
}

bool LanguagesManager::isSupported(const QString &languageID) {
    return itemExists(languageID);
}
