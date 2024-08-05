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

LocalConfig::TimeFormat LocalConfig::timeFormat() {
    if(!_currentTimeFormatInitialized) {
        int value = settingsGeneral.value("TimeFormat", 24).toInt();
        if(value == 12) {
            _currentTimeFormat = Hours12;
            return Hours12;
        } else {
            _currentTimeFormat = Hours24;
            return Hours24;
        }
    }

    return _currentTimeFormat;
}

void LocalConfig::setTimeFormat(const TimeFormat &newTimeFormat) {
    _currentTimeFormat = newTimeFormat;
    settingsGeneral.setValue("TimeFormat", newTimeFormat == Hours12 ? 12 : 24);
}

QString LocalConfig::timeFormatString(const bool &showSeconds, const bool &oneDigitHour) {
    TimeFormat format = timeFormat();
    if(format == Hours12)
        if(oneDigitHour)
            return showSeconds ? "h:mm:ss A" : "h:mm A";
        else
            return showSeconds ? "hh:mm:ss A" : "hh:mm A";
    else
        if(oneDigitHour)
            return showSeconds ? "h:mm:ss" : "h:mm";
        else
            return showSeconds ? "hh:mm:ss" : "hh:mm";
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

Qt::ToolBarArea LocalConfig::workspacesToolbarPosition() {
    int i = settingsGeneral.value("workspacesToolbarPosition").toInt();
    switch(i) {
        case 0x1: return Qt::LeftToolBarArea;
        case 0x2: return Qt::RightToolBarArea;
        case 0x4: return Qt::TopToolBarArea;
        case 0x8: return Qt::BottomToolBarArea;
    }

    return Qt::TopToolBarArea;
}

void LocalConfig::setWorkspacesToolbarPosition(const Qt::ToolBarArea &position) {
    settingsGeneral.setValue("workspacesToolbarPosition", position);
}
