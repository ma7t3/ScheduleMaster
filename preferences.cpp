#include "preferences.h"

QSettings Preferences::settingsGeneral = QSettings("ScheduleMaster", "general");

Preferences::Preferences(QObject *parent) :
    QObject(parent) {
}

int Preferences::language() {
    QString lang = settingsGeneral.value("Language").toString();

    if(lang == "DE-de")
        return LanguageGerman;
    else return LanguageEnglish;
}

void Preferences::setLanguage(const int &newLanguage) {
    settingsGeneral.setValue("Language", newLanguage == 1 ? "DE-de" : "EN-us");
}
