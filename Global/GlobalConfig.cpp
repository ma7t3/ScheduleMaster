#include "GlobalConfig.h"

GlobalConfig::GlobalConfig() : QObject(nullptr) {}

GlobalConfig *GlobalConfig::instance() {
    static GlobalConfig instance;
    return &instance;
}

void GlobalConfig::init() {
    qInfo() << "Loading global configuration...";

    loadSupportedLanguages();
    loadNativeFolderLocations();
}

QList<QLocale> GlobalConfig::supportedLanguages() {
    return _supportedLanguages.values();
}

bool GlobalConfig::languageIsSupported(const QLocale &language) {
    return _supportedLanguages.contains(language);
}

bool GlobalConfig::languageIsSupported(const QString &languageName) {
    return languageIsSupported(QLocale(languageName));
}

void GlobalConfig::addSupportedLanguage(const QLocale &newLanguage) {
    _supportedLanguages.insert(newLanguage);
}

QList<FolderLocation> GlobalConfig::folderLocations() {
    return _folderLocations.values();
}

void GlobalConfig::setFolderLocationName(const QString &id, const QString &name) {
    if(!_folderLocations.contains(id))
        return;

    _folderLocations[id].name = name;
}

QJsonArray GlobalConfig::loadConfigResource(const QString &resource) {
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

void GlobalConfig::loadSupportedLanguages() {
    qInfo() << "   Loading supported languages...";
    QJsonArray languages = loadConfigResource("Languages");
    for(const QJsonValue &val : std::as_const(languages)) {
        QString lang = val.toString();
        QLocale locale(lang);
        _supportedLanguages << locale.language();
        qDebug().noquote() << "      - " + lang;
    }
}

void GlobalConfig::loadNativeFolderLocations() {
    qInfo() << "   Loading folder locations...";
    QJsonArray locations = loadConfigResource("Locations");
    for(const QJsonValue &val : std::as_const(locations)) {
        QJsonObject obj = val.toObject();

        QString id = obj.value("id").toString();
        QString icon = obj.value("icon").toString();
        bool multiple = obj.value("multiple").toBool();

        // fallback if no name is given
        QString name = id;

        if(id == "base.projectFilesDefault")
            name = tr("Default Location for Project Files");
        else if(id == "base.logfile")
            name = tr("Logfiles");
        else if(id == "base.plugins")
            name = tr("Plugins");

        FolderLocation location(id, name, icon, multiple);

        _folderLocations.insert(id, location);
        qDebug().noquote() << "      - " + id;
    }
}
