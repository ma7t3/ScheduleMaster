#include "localconfig.h"

QSettings LocalConfig::settingsGeneral = QSettings("ScheduleMaster", "general");

LocalConfig::LocalConfig(QObject *parent) :
    QObject(parent) {
}

int LocalConfig::language() {
    QString lang = settingsGeneral.value("Language", "EN-us").toString();

    if(lang == "DE-de")
        return LanguageGerman;
    else return LanguageEnglish;
}

void LocalConfig::setLanguage(const int &newLanguage) {
    settingsGeneral.setValue("Language", newLanguage == 1 ? "DE-de" : "EN-us");
}
