#include "GlobalConfig.h"

GlobalConfig::GlobalConfig() : QObject(nullptr) {}

GlobalConfig *GlobalConfig::instance() {
    static GlobalConfig instance;
    return &instance;
}

void GlobalConfig::initLanguages() {
    qInfo() << "Loading global configuration (1/2)...";

    loadSupportedLanguages();
}

void GlobalConfig::init() {
    qInfo() << "Loading global configuration (2/2)...";

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

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &error);
        f.close();

        if(error.error != QJsonParseError::NoError)
            qWarning().noquote() << "Error while parsing resource configuration file: \"" + f.fileName() + "\" (" + error.errorString() + ")";

        QJsonArray rawArray = doc.array();
        QJsonArray array = resolveConfigResourceTranslatedStrings(rawArray);
        for(const QJsonValue &val : std::as_const(array))
            data.append(val);
    }

    return data;
}

QJsonValue GlobalConfig::resolveConfigResourceTranslatedStrings(QJsonObject jsonObject) {
    if(jsonObject.value("object").toString() == "translated_string") {
        QLocale l;
        QString localeName = l.name();

        QString resultString = "";

        if(jsonObject.contains(localeName))
            resultString = jsonObject.value(localeName).toString();
        else if(jsonObject.contains("en_US"))
            resultString = jsonObject.value("en_US").toString();

        return resultString;
    }

    for(const QString &key : jsonObject.keys()) {
        QJsonValue value = jsonObject.value(key);

        if(value.isObject())
            jsonObject.insert(key, resolveConfigResourceTranslatedStrings(value.toObject()));
        else if(value.isArray())
            jsonObject.insert(key, resolveConfigResourceTranslatedStrings(value.toArray()));
    }

    return jsonObject;
}

QJsonArray GlobalConfig::resolveConfigResourceTranslatedStrings(QJsonArray jsonArray) {
    for(int i = 0; i < jsonArray.count(); i++) {
        QJsonValue value = jsonArray[i];

        if(value.isObject())
            jsonArray.replace(i, resolveConfigResourceTranslatedStrings(value.toObject()));
        else if(value.isArray())
            jsonArray.replace(i, resolveConfigResourceTranslatedStrings(value.toArray()));
    }

    return jsonArray;
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
        QString name = obj.value("name").toString();
        QString icon = obj.value("icon").toString();
        bool multiple = obj.value("multiple").toBool();

        // fallback if no name is given
        if(name.isEmpty())
            name = id;

        FolderLocation location(id, name, icon, multiple);

        _folderLocations.insert(id, location);
        qDebug().noquote() << "      - " + id;
    }
}
