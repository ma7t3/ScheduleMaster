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

    loadFolderLocations();
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

QList<FolderLocation> GlobalConfig::folderLocations() {
    return _folderLocations.values();
}

QJsonDocument GlobalConfig::loadSingleConfigResource(const QString &resource) {
    return parseJsonFile(":/Config/" + resource + ".json");
}

QJsonArray GlobalConfig::loadMultiConfigResource(const QString &resource) {
    QJsonArray data;

    const QDir dir(":/Config/" + resource);
    const QStringList entrys = dir.entryList();
    for(const QString &entry : entrys) {
        const QJsonDocument doc = parseJsonFile(dir.path() + "/" + entry);
        if(doc.isNull())
            continue;

        const QJsonArray array = doc.array();
        for(const QJsonValue &val : array)
            data.append(val);
    }

    return data;
}

QJsonDocument GlobalConfig::parseJsonFile(const QString &fileName) {
    QFile f(fileName);
    if(!f.open(QIODevice::ReadOnly))
        return QJsonDocument();

    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &error);
    f.close();

    if(error.error != QJsonParseError::NoError) {
        qWarning().noquote() << "Error while parsing resource configuration file: \"" + fileName + "\" (" + error.errorString() + ")";
        return QJsonDocument();
    }

    if(doc.isArray())
        return QJsonDocument(resolveTranslatedStrings(doc.array()));
    else if(doc.isObject())
        return QJsonDocument(resolveTranslatedStrings(doc.object()).toObject());
    else {
        qWarning().noquote() << "Error while loading resource configuration file: \"" + fileName + "\"";
        return QJsonDocument();
    }
}

QJsonValue GlobalConfig::resolveTranslatedStrings(QJsonObject jsonObject) {
    if(jsonObject.value("object").toString() == "translated_string") {
        const QLocale l;
        const QString localeName = l.name();

        QString resultString;

        if(jsonObject.contains(localeName))
            resultString = jsonObject.value(localeName).toString();
        else if(jsonObject.contains("en_US"))
            resultString = jsonObject.value("en_US").toString();

        return resultString;
    }

    const QStringList keys = jsonObject.keys();
    for(const QString &key : keys) {
        QJsonValue value = jsonObject.value(key);

        if(value.isObject())
            jsonObject.insert(key, resolveTranslatedStrings(value.toObject()));
        else if(value.isArray())
            jsonObject.insert(key, resolveTranslatedStrings(value.toArray()));
    }

    return jsonObject;
}

QJsonArray GlobalConfig::resolveTranslatedStrings(QJsonArray jsonArray) {
    for(int i = 0; i < jsonArray.count(); i++) {
        const QJsonValue value = jsonArray[i];

        if(value.isObject())
            jsonArray.replace(i, resolveTranslatedStrings(value.toObject()));
        else if(value.isArray())
            jsonArray.replace(i, resolveTranslatedStrings(value.toArray()));
    }

    return jsonArray;
}

void GlobalConfig::loadSupportedLanguages() {
    qInfo() << "   Loading supported languages...";
    const QJsonArray languages = loadMultiConfigResource("Languages");
    for(const QJsonValue &val : languages) {
        const QString lang = val.toString();
        const QLocale locale(lang);
        _supportedLanguages << locale.language();
        qDebug().noquote() << "      - " + lang;
    }
}

void GlobalConfig::loadFolderLocations() {
    qInfo() << "   Loading folder locations...";
    const QJsonArray locations = loadMultiConfigResource("Locations");
    for(const QJsonValue &val : std::as_const(locations)) {
        const QJsonObject obj = val.toObject();

        const QString id   = obj.value("id").toString();
              QString name = obj.value("name").toString();
        const QString icon = obj.value("icon").toString();
        bool multiple = obj.value("multiple").toBool();

        // fallback if no name is given
        if(name.isEmpty())
            name = id;

        const FolderLocation location(id, name, icon, multiple);

        _folderLocations.insert(id, location);
        qDebug().noquote() << "      - " + id;
    }
}
