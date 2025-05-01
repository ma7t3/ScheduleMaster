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

    qInfo() << "   Loading current language...";
    QString str = read("general.language").toString();
    QLocale tmpLocale(str);
    if(str == "system" || !LanguagesHandler::isSupported(tmpLocale.name()))
        _locale = QLocale::system();
    else
        _locale = QLocale(str);

    QLocale::setDefault(_locale);
}

void LocalConfig::init() {
    qInfo() << "Loading local configuration (2/2)...";
}

QVariant LocalConfig::read(const QString &id) {
    QVariant value = readSilent(id);
    qDebug().noquote() << "Read setting: " << id << " << " << value;
    return value;
}

QVariant LocalConfig::readSilent(const QString &id) {
    QVariant value;
    if(GlobalConfig::settingsItemExists(id)) {
        SettingsItem item = GlobalConfig::settingsItem(id);
        QVariant rawValue = settings.value(id, item.defaultValue);
        value = VariantConverter::convert(rawValue, item.type);
        if(!keyExsists(id) && !item.isGroup)
            write(id, value); // init with default value

        return value;
    } else {
        return settings.value(id);
    }
}

QVariant LocalConfig::write(const QString &id, const QVariant &value) {
    bool itemExists = GlobalConfig::settingsItemExists(id);

    QVariant convVal;

    if(itemExists)
        convVal = VariantConverter::convert(value, static_cast<QMetaType::Type>(GlobalConfig::settingsItem(id).type));
    else
        convVal = value;

    if(keyExsists(id)) {
        if(readSilent(id) == convVal)
            return QVariant();
    }

    bool restartRequired = GlobalConfig::settingRequiresRestart(id);
    if(restartRequired)
        _modifiedRestartRequiredSettings << id;

    qDebug().noquote() << "Save setting: " << id << " = " << convVal << (restartRequired ? " (requires restart)" : "");
    bool exists = settings.contains(id);
    settings.setValue(id, convVal);

    if(exists)
        emit instance()->settingChanged(id, value);
    else
        emit instance()->settingAdded(id, value);

    if(id.startsWith("keyboardShortcuts/"))
        emit instance()->keyboardShortcutChanged(QString(id).remove("keyboardShortcuts/"), value.value<QKeySequence>());

    return convVal;
}

void LocalConfig::remove(const QString &id) {
    bool restartRequired = GlobalConfig::settingRequiresRestart(id);

    if(restartRequired)
        _modifiedRestartRequiredSettings << id;

    qDebug().noquote() << "Remove setting: " << id << (restartRequired ? " (requires restart)" : "");
    settings.remove(id);
    emit instance()->settingRemoved(id);
}

QVariant LocalConfig::restoreDefault(const QString &id) {
    if(!GlobalConfig::settingsItemExists(id)) {
        qWarning().noquote() << "Cannot reset setting to default: " << id << " - setting is not registered to ScheduleMaster!";
        return QVariant();
    }

    qDebug().noquote() << "Reset setting to default: " << id;
    return write(id, GlobalConfig::settingsItem(id).defaultValue);
}

void LocalConfig::rename(const QString &oldID, const QString &newID) {
    QVariant value = read(oldID);
    write(newID, value);
    remove(oldID);
}

QStringList LocalConfig::groupKeys(const QString &group) {
    settings.beginGroup(group);
    QStringList keys = settings.childKeys();
    settings.endGroup();
    return keys;
}

QStringList LocalConfig::groupSubGroups(const QString &group) {
    settings.beginGroup(group);
    QStringList groups = settings.childGroups();
    settings.endGroup();
    return groups;
}

bool LocalConfig::keyExsists(const QString &key) {
    return settings.contains(key);
}

QStringList LocalConfig::allKeys() {
    return settings.childKeys();
}

QStringList LocalConfig::allGroups() {
    return settings.childGroups();
}

bool LocalConfig::restartRequired() {
    return !_modifiedRestartRequiredSettings.isEmpty();
}

QStringList LocalConfig::restartRequiredSettings() {
    QStringList list = _modifiedRestartRequiredSettings.values();
    list.sort();
    return list;
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
    write("general.language", name);
}

QString LocalConfig::style() {
    return read("appearance.theme").toString();
}

void LocalConfig::setStyle(const QString &newStyle) {
    if(!GlobalConfig::styleExists(newStyle))
        return;

    write("appearance.theme", newStyle);
    emit instance()->styleChanged(newStyle);
}

Qt::ColorScheme LocalConfig::colorScheme() {
    int intVal = read("appearance.colorScheme").toInt();
    if(intVal < 0 || intVal > 2)
        return Qt::ColorScheme::Unknown;

    return static_cast<Qt::ColorScheme>(intVal);
}

void LocalConfig::setColorScheme(const Qt::ColorScheme &colorscheme) {
    write("appearance.colorScheme", static_cast<int>(colorscheme));
    emit instance()->colorSchemeChanged(colorscheme);
}

QString LocalConfig::uiFontFamily() {
    QString str = read("appearance.font").toString();
    if(str.isEmpty())
        return qGuiApp->font().family();
    else
        return str;
}

void LocalConfig::setUiFontFamily(const QString &fontFamily) {
    write("appearance.font", fontFamily);
    emit instance()->uiFontChanged(fontFamily);
}

QString LocalConfig::accentColorID() {
    return read("appearance.accentColor").toString();
}

QColor LocalConfig::accentColor() {
    const QString id = accentColorID();
    return GlobalConfig::accentColor(id);
}

void LocalConfig::setAccentColor(const QString &id) {
    write("appearance.accentColor", id);
    emit instance()->accentColorChanged(id);
}

bool LocalConfig::useGdiEngine() {
    return read("appearance.fontEngineGDI").toBool();
}

void LocalConfig::setUseGdiEngine(const bool &useGdiEngine) {
    write("appearance.fontEngineGDI", useGdiEngine);
}

QMap<QString, QStringList> LocalConfig::folderLocations() {
    QMap<QString, QStringList> data;

    const QStringList keys = groupKeys("locations");

    const QStringList standardKeys = GlobalConfig::folderLocationIDs();
    for(const QString &key : standardKeys) {
        if(!keys.contains(key)) {
            data[key] = GlobalConfig::settingsItem("locations/" + key).defaultValue.toStringList();
            write("locations/" + key, data[key]);
        }
    }

    for(const QString &id : keys)
        data[id] = folderLocationPaths(id);

    return data;
}

void LocalConfig::setFolderLocations(const QMap<QString, QStringList> &locations) {
    QStringList locationKeys = locations.keys();
    for(const QString &key : std::as_const(locationKeys))
        write("locations/" + key, locations.value(key));
}

QStringList LocalConfig::folderLocationPaths(const QString &id) {
    QStringList values = read("locations/" + id).toStringList();
    if(values.empty()) {
        values = GlobalConfig::settingsItem("locations/" + id).defaultValue.toStringList();
        if(values.isEmpty() && id == "logfile")
            values = {GlobalConfig::defaultLogfileLocation()};
        write("locations/" + id, values);
    }

    if(values.isEmpty())
        values << "";

    return values;
}

void LocalConfig::setFolderLocationPaths(const QString &id, const QStringList &paths) {
    write("locations/" + id, paths);
}

QKeySequence LocalConfig::keyboardShortcut(const QString &id) {
    QString fullID = "keyboardShortcuts/" + id;
    if(keyExsists(fullID))
        return QKeySequence(read(fullID).toString());

    if(!GlobalConfig::keyboardShortcutExists(id))
        return QKeySequence();

    QKeySequence sequence = GlobalConfig::keyboardShortcutDefaultKeySequence(id);
    setKeyboardShortcut(id, sequence);
    return sequence;
}

void LocalConfig::setKeyboardShortcut(const QString &id, const QKeySequence &keySequence) {
    write("keyboardShortcuts/" + id, keySequence.toString(QKeySequence::PortableText));
}

QStringList LocalConfig::lastUsedFiles() {
    return read("general.lastUsedFiles").toStringList();
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
    write("general.lastUsedFiles", modifiedList);
    emit instance()->lastUsedFilesChanged();
}

void LocalConfig::removeLastUsedFile(const QString &filePath) {
    QStringList list = lastUsedFiles();
    list.removeAll(filePath);
    setLastUsedFiles(list);
}

void LocalConfig::resetLastUsedFiles() {
    write("general.lastUsedFiles", QStringList());
}

bool LocalConfig::crashDetected() {
    return !read("general.closeCheck").toBool();
}

void LocalConfig::setCrashDetected(const bool &newCrashDetected) {
    write("general.closeCheck", !newCrashDetected);
}

QString LocalConfig::lastLogfileName() {
    return _lastLogfileName;
}

void LocalConfig::setLastLogfileName(const QString &newLastLogfileName) {
    _lastLogfileName = newLastLogfileName;
}

bool LocalConfig::currentLogfileExists() {
    return _logfileCreated;
}

LocalConfig::LogfileMode LocalConfig::logfileMode() {
    bool ok;
    int i = read("general.logfileMode").toInt(&ok);
    if(!ok)
        return DefaultLog;

    if(i == DebugDetailLog)
        return DebugDetailLog;

    if(i == DebugLog)
        return DebugLog;

    if(i == NoLog)
        return NoLog;

    _logfileCreated = (i != NoLog);

    return DefaultLog;
}

void LocalConfig::setLogfileMode(const LogfileMode &newLogmode) {
    write("general.logfileMode", newLogmode);
}

Qt::ToolBarArea LocalConfig::workspacesToolbarPosition() {
    int i = read("appearance.workspacesToolbarPosition").toInt();
    switch(i) {
    case 0x1: return Qt::LeftToolBarArea;
    case 0x2: return Qt::RightToolBarArea;
    case 0x4: return Qt::TopToolBarArea;
    case 0x8: return Qt::BottomToolBarArea;
    }

    return Qt::TopToolBarArea;
}

void LocalConfig::setWorkspacesToolbarPosition(const Qt::ToolBarArea &position) {
    write("appearance.workspacesToolbarPosition", position);
}

QByteArray LocalConfig::mainWindowGeometry() {
    return read("general.mainWindowGeometry").toByteArray();
}

void LocalConfig::setMainWindowGeomentry(const QByteArray &geometry) {
    write("general.mainWindowGeometry", geometry);
}

void LocalConfig::importKeyboardShortcutsFromJson(const QJsonArray &jsonArray) {
    for(const QJsonValue &value : jsonArray) {
        const QJsonObject jObj = value.toObject();
        const QString id          = jObj.value("id").toString();
        const QString keySequence = jObj.value("keySequence").toString();
        setKeyboardShortcut(id, keySequence);
    }
}

QJsonArray LocalConfig::exportKeyboardShortcutsToJson() {
    QJsonArray array;
    const QStringList keys = groupKeys("keyboardShortcuts");
    for(const QString &id : keys) {
        QJsonObject obj;
        obj["id"] = id;
        obj["keySequence"] = keyboardShortcut(id).toString();
        array << obj;
    }
    return array;
}
