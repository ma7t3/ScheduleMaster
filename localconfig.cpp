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

QStringList LocalConfig::lastUsedFiles() {
    return settingsGeneral.value("lastUsedFiles", {}).toStringList();
}

void LocalConfig::addLastUsedFile(const QString &filePath) {
    QStringList list = lastUsedFiles();

    if(list.contains(filePath))
        list.removeAll(filePath);

    list.insert(0, filePath);

    setLastUsedFiles(list);
}

void LocalConfig::setLastUsedFiles(const QStringList &list) {
    settingsGeneral.setValue("lastUsedFiles", list);
}

void LocalConfig::resetLastUsedFiles() {
    settingsGeneral.setValue("lastUsedFiles", {});
}

bool LocalConfig::compressFiles() {
    return settingsGeneral.value("compressFiles", true).toBool();
}

void LocalConfig::setCompressFiles(const bool &newCompressFiles) {
    settingsGeneral.setValue("compressFiles", newCompressFiles);
}
