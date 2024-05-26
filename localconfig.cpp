#include "localconfig.h"


LocalConfig::LocalConfig(QObject *parent) :
    QObject(parent) {
}

LocalConfig::Language LocalConfig::language() {
    QString lang = settingsGeneral.value("Language", "EN-us").toString();

    if(lang == "DE-de")
        return German;
    else return English;
}

void LocalConfig::setLanguage(const Language &newLanguage) {
    settingsGeneral.setValue("Language", newLanguage == 1 ? "DE-de" : "EN-us");
}

LocalConfig::Style LocalConfig::style() {
    QString str = settingsGeneral.value("style").toString();
    if(str == "Fusion")
        return Fusion;
    else if(str == "WindowsXpStyle")
        return WindowsXpStyle;
    else
        return System;
}

void LocalConfig::setStyle(const Style &newStyle) {
    QString str;
    if(newStyle == Fusion)
        str = "Fusion";
    else if(newStyle == WindowsXpStyle)
        str = "WindowsXpStyle";
    else
        str = "System";
    settingsGeneral.setValue("style", str);
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

bool LocalConfig::crashDetected() {
    return !settingsGeneral.value("closeCheck", true).toBool();
}

void LocalConfig::setCrashDetected(const bool &newCrashDetected) {
    settingsGeneral.setValue("closeCheck", !newCrashDetected);
}

QString LocalConfig::lastLogfileName() {
    return _lastLogfileName;
}

void LocalConfig::setLastLogfileName(const QString &newLastLogfileName) {
    _lastLogfileName = newLastLogfileName;
}

QString LocalConfig::defaultProjectLocation() {
    return settingsGeneral.value("defaultProjectLocation", QDir::homePath() + "/.ScheduleMaster/Projects").toString();
}

void LocalConfig::setDefaultProjectLocation(const QString &newDefaultProjectLocation) {
    settingsGeneral.setValue("defaultProjectLocation", newDefaultProjectLocation);
}

LocalConfig::LogfileMode LocalConfig::logfileMode() {
    bool ok;
    int i = settingsGeneral.value("logfileMode", DefaultLog).toInt(&ok);
    if(!ok)
        return DefaultLog;

    if(i == DebugDetailLog)
        return DebugDetailLog;

    if(i == DebugLog)
        return DebugLog;

    if(i == NoLog)
        return NoLog;

    return DefaultLog;
}

void LocalConfig::setLogfileMode(const LogfileMode &newLogmode) {
    settingsGeneral.setValue("logfileMode", newLogmode);
}
