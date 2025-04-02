#include "LocalConfig.h"

#include <QDir>

LocalConfig::LocalConfig() : QObject(nullptr) {}

void LocalConfig::init() {
    QString str = settingsGeneral.value("language", "system").toString();
    QLocale tmpLocale(str);
    if(str == "system" || !_supportedLanguages.contains(tmpLocale.language()))
        _locale = QLocale::system();
    else
        _locale = QLocale(str);
}

LocalConfig *LocalConfig::instance() {
    static LocalConfig instance;
    return &instance;
}

QList<QLocale::Language> LocalConfig::supportedLanguages() {
    return _supportedLanguages;
}

void LocalConfig::addSupportedLanguage(const QLocale::Language &newLanguage) {
    _supportedLanguages << newLanguage;
}

QLocale LocalConfig::locale() {
    return _locale;
}

QLocale::Language LocalConfig::language() {
    return locale().language();
}

void LocalConfig::setLanguage(const QLocale::Language &newLanguage) {
    setLanguage(QLocale(newLanguage).name());
}

void LocalConfig::setLanguage(const QString &name) {
    settingsGeneral.setValue("language", name);
}

LocalConfig::Style LocalConfig::style() {
    QString str = settingsGeneral.value("style").toString();
    if(str == "Fusion")
        return FusionStyle;
    else if(str == "WindowsXpStyle")
        return WindowsXpStyle;
    else
        return SystemStyle;
}

void LocalConfig::setStyle(const Style &newStyle) {
    QString str;
    if(newStyle == FusionStyle)
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
    list.removeAll(filePath);
    list.insert(0, filePath);
    setLastUsedFiles(list);
}

void LocalConfig::setLastUsedFiles(const QStringList &list) {
    QStringList modifiedList = list;
    modifiedList.replaceInStrings("\\", "/");
    settingsGeneral.setValue("lastUsedFiles", modifiedList);
    emit instance()->lastUsedFilesChanged();
}

void LocalConfig::removeLastUsedFile(const QString &filePath) {
    QStringList list = lastUsedFiles();
    list.removeAll(filePath);
    setLastUsedFiles(list);
}

void LocalConfig::resetLastUsedFiles() {
    settingsGeneral.setValue("lastUsedFiles", QStringList());
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

QByteArray LocalConfig::mainWindowGeometry() {
    return settingsGeneral.value("mainWindowGeometry").toByteArray();
}

void LocalConfig::setMainWindowGeomentry(const QByteArray &geometry) {
    settingsGeneral.setValue("mainWindowGeometry", geometry);
}
