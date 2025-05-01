#include "LanguagesHandler.h"

Language::Language(const QString &id) : GlobalConfigItem(id) {}

Language::Language(const QJsonObject &json)
    : GlobalConfigItem(json), locale(id()) {}

LanguagesHandler::LanguagesHandler(QObject *parent) : GlobalConfigHandler(parent) {
    loadItems("Languages");
}

void LanguagesHandler::init() {
    instance();
}

QList<Language> LanguagesHandler::supportedLanguages() {
    return instance()->items();
}

bool LanguagesHandler::isSupported(const QString &languageID) {
    return instance()->itemExists(languageID);
}

LanguagesHandler *LanguagesHandler::instance() {
    static LanguagesHandler handler(nullptr);
    return &handler;
}
