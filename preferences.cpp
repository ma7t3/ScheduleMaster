#include "preferences.h"

Preferences::Preferences(QObject *parent) :
    QObject(parent) {
    QString lang = settingsGeneral.value("Language").toString();

    if(lang == "DE-de")
        _language = LanguageGerman;
    else _language = LanguageEnglish;
}

int Preferences::language() const {
    return _language;
}

void Preferences::setLanguage(int newLanguage) {
    _language = newLanguage;
    settingsGeneral.setValue("Language", newLanguage == 1 ? "DE-de" : "EN-us");
}
