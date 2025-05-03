#include "GlobalConfig.h"




GlobalConfig::GlobalConfig() : QObject(nullptr) {}

GlobalConfig *GlobalConfig::instance() {
    static GlobalConfig instance;
    return &instance;
}

void GlobalConfig::init() {
    qInfo() << "Loading global configuration (2/2)...";
    loadAccentColors();
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
    if(!f.open(QIODevice::ReadOnly)) {
        qWarning().noquote() << "Error while reading resource configuration file: \"" + fileName + "\" (" + f.errorString() + ")";
        return QJsonDocument();
    }

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

void GlobalConfig::loadAccentColors() {
    qInfo() << "   Loading accent colors...";
    const QJsonDocument doc = loadSingleConfigResource("accentColors");
    const QJsonObject object = doc.object();
    const QStringList keys = object.keys();
    for(const QString &key : keys)
        _accentColors.insert(key, QColor(object[key].toString()));
}
