#include "LocalConfig.h"

#include <QDir>

LocalConfig::LocalConfig() : QObject(nullptr) {}

LocalConfig *LocalConfig::instance() {
    static LocalConfig instance;
    return &instance;
}

void LocalConfig::init() {
    qInfo() << "Loading configuration...";

    loadSupportedLanguages();
    loadNativeFolderLocations();

    QString str = settingsGeneral.value("language", "system").toString();
    QLocale tmpLocale(str);
    if(str == "system" || !_supportedLanguages.contains(tmpLocale.language()))
        _locale = QLocale::system();
    else
        _locale = QLocale(str);
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

QList<FolderLocation> LocalConfig::folderLocations() {
    // update from registry
    for(FolderLocation &loc : _folderLocations) {
        QStringList paths = settingsLocations.value(loc.id, {}).toStringList();
        if(!paths.isEmpty())
            loc.paths = paths;
    }

    return _folderLocations.values();
}

QStringList LocalConfig::folderLocationPaths(const QString &id) {
    return settingsLocations.value(id, {}).toStringList();
}

void LocalConfig::updateFolderLocation(const FolderLocation &location) {
    if(!_folderLocations.contains(location.id))
        _folderLocations.insert(location.id, location);
    else {
        FolderLocation oldLoc = _folderLocations[location.id];
        FolderLocation newLoc = location;

        // don't overwrite name or icon with empty values
        if(location.name.isEmpty())
            newLoc.name = oldLoc.name;
        if(location.icon.isEmpty())
            newLoc.icon = oldLoc.icon;

        _folderLocations.insert(location.id, location);
    }

    settingsLocations.setValue(location.id, location.paths);
}

void LocalConfig::setFolderLocationName(const QString &id, const QString &name) {
    if(_folderLocations.contains(id))
        _folderLocations[id].name = name;
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


QJsonArray LocalConfig::loadConfigResource(const QString &resource) {
    QJsonArray data;

    QDir dir(":/Config/" + resource);
    QStringList entrys = dir.entryList();
    for(const QString &entry : std::as_const(entrys)) {
        QFile f(dir.path() + "/" + entry);
        if(!f.open(QIODevice::ReadOnly))
            continue;

        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        f.close();

        QJsonArray array = doc.array();
        for(const QJsonValue &val : std::as_const(array))
            data.append(val);
    }

    return data;
}

void LocalConfig::loadSupportedLanguages() {
    qInfo() << "   Loading supported languages...";
    QJsonArray languages = loadConfigResource("Languages");
    for(const QJsonValue &val : std::as_const(languages)) {
        QString lang = val.toString();
        QLocale locale(lang);
        _supportedLanguages << locale.language();
        qDebug().noquote() << "      - " + lang;
    }
}

void LocalConfig::loadNativeFolderLocations() {
    qInfo() << "   Loading folder locations...";
    QJsonArray locations = loadConfigResource("Locations");
    for(const QJsonValue &val : std::as_const(locations)) {
        QJsonObject obj = val.toObject();

        QString id = obj.value("id").toString();
        QString icon = obj.value("icon").toString();
        bool multiple = obj.value("multiple").toBool();

        // fallback if no name is given
        QString name = id;

        QStringList paths = folderLocationPaths(id);

        if(id == "base.projectFilesDefault") {
            name = tr("Default Location for Project Files");
            if(paths.isEmpty())
                paths = {QDir::homePath() + "/ScheduleMaster/Projects"};

        } else if(id == "base.logfile") {
            name = tr("Logfiles");
            if(paths.isEmpty())
                paths = {QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/logs"};

        } else if(id == "base.plugins") {
            name = tr("Plugins");
            if(paths.isEmpty())
                paths = {QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/plugins", QCoreApplication::applicationDirPath() + "/plugins"};
        }

        qDebug() << paths;


        QString singlePath = paths.isEmpty() ? "" : paths.first();

        FolderLocation location(id, name, icon, multiple, (multiple ? paths : (QStringList() << singlePath)));

        _folderLocations.insert(id, location);
        qDebug().noquote() << "      - " + id + " (" + (paths.isEmpty() ? "empty" : paths.join(", ")) + ")";
    }
}
