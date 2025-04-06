#include "LocalConfig.h"

#include <QDir>

// TODO: Review everything and fix registry reading/saving + default values

LocalConfig::LocalConfig() : QObject(nullptr) {}

LocalConfig *LocalConfig::instance() {
    static LocalConfig instance;
    return &instance;
}

void LocalConfig::initLocale() {
    qInfo() << "Loading local configuration (1/2)...";

    QString str = settingsGeneral.value("language", "system").toString();
    QLocale tmpLocale(str);
    if(str == "system" || !GlobalConfig::languageIsSupported(tmpLocale.language()))
        _locale = QLocale::system();
    else
        _locale = QLocale(str);

    QLocale::setDefault(_locale);

}

void LocalConfig::init() {
    qInfo() << "Loading local configuration (2/2)...";

    loadFolderLocations();
}

QLocale LocalConfig::locale() {
    return _locale;
}

QLocale::Language LocalConfig::language() {
    return locale().language();
}

void LocalConfig::setLanguage(const QLocale &newLanguage) {
    setLanguage(newLanguage.name());
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

QMap<QString, QStringList> LocalConfig::folderLocations() {
    QMap<QString, QStringList> data;

    for(const QString &id : _folderLocations.keys())
        data[id] = folderLocationPaths(id);

    return data;
}

void LocalConfig::setFolderLocations(const QMap<QString, QStringList> &locations) {
    _folderLocations = locations;

    for(const QString &key : locations.keys())
        settingsLocations.setValue(key, locations.value(key));
}

QStringList LocalConfig::folderLocationPaths(const QString &id) {
    QStringList paths = settingsLocations.value(id, {}).toStringList();
    _folderLocations.insert(id, paths);

    return paths;
}

void LocalConfig::setFolderLocationPaths(const QString &id, const QStringList &paths) {
    _folderLocations.insert(id, paths);
    settingsLocations.setValue(id, paths);
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

void LocalConfig::loadFolderLocations() {
    qInfo() << "   Loading folder locations...";

    QStringList keys = settingsLocations.allKeys();
    for(const QString &key : std::as_const(keys)) {
        QStringList value = settingsLocations.value(key).toStringList();
        _folderLocations.insert(key, value);
        qDebug().noquote() << "      - [" + key + "] (" + value.join(", ") + ")";
    }

    for(FolderLocation &loc : GlobalConfig::folderLocations()) {
        QStringList paths = folderLocationPaths(loc.id);

        if(!settingsLocations.contains(loc.id)) {
            if(loc.id == "base.projectFilesDefault")
                paths = {QDir::homePath() + "/ScheduleMaster/Projects"};
            else if(loc.id == "base.logfile")
                paths = {QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/logs"};
            else if(loc.id == "base.plugins")
                paths = {QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/plugins", QCoreApplication::applicationDirPath() + "/plugins"};

            setFolderLocationPaths(loc.id, paths);
            qDebug().noquote() << "      - [" + loc.id + "] (" + paths.join(", ") + ")";
        }
    }
}
