#include "GlobalConfig.h"

Style::Style(const QJsonObject &jsonObject) : GlobalConfigItem(jsonObject) {
    name               = jsonObject.value("name").toString(id());
    lightSupport       = jsonObject.value("lightSupport").toBool();
    darkSupport        = jsonObject.value("darkSupport").toBool();
    accentColorSupport = jsonObject.value("accentColorSupport").toBool();
    applyPalette       = jsonObject.value("applyPalette").toBool();

    QString typeString = jsonObject.value("type").toString();
    type = typeString == "systemDefault" ? SystemDefaultType
         : typeString == "styleClass"    ? StyleClassType
         : typeString == "styleSheet"    ? StyleSheetType
         : typeString == "styleFactory"  ? StyleFactoryType
                                         : InvalidType;

    switch(type) {
        case StyleClassType:   styleClassID     = jsonObject.value("styleClassID").toString();     break;
        case StyleSheetType:   styleSheetUrl    = jsonObject.value("styleSheetUrl").toString();    break;
        case StyleFactoryType: styleFactoryName = jsonObject.value("styleFactoryName").toString(); break;
        default: break;
    }
}

Style::Style(const QString &id) : GlobalConfigItem(id) {}

bool Style::supportsColorScheme(const Qt::ColorScheme &colorScheme) const {
    return (
        (colorScheme == Qt::ColorScheme::Unknown && lightSupport && darkSupport) ||
        (colorScheme == Qt::ColorScheme::Light && lightSupport) ||
        (colorScheme == Qt::ColorScheme::Dark && darkSupport)
    );
}

QList<Qt::ColorScheme> Style::supportedColorSchemes() const {
    if(supportsColorScheme(Qt::ColorScheme::Unknown))
        return {Qt::ColorScheme::Unknown, Qt::ColorScheme::Light, Qt::ColorScheme::Dark};

    if(supportsColorScheme(Qt::ColorScheme::Light))
        return {Qt::ColorScheme::Light};

    if(supportsColorScheme(Qt::ColorScheme::Dark))
        return {Qt::ColorScheme::Dark};

    return {};
}


GlobalConfig::GlobalConfig() : QObject(nullptr) {}

GlobalConfig *GlobalConfig::instance() {
    static GlobalConfig instance;
    return &instance;
}

void GlobalConfig::init() {
    qInfo() << "Loading global configuration (2/2)...";

    loadStyles();
    loadAccentColors();
}

QMap<QString, Style> GlobalConfig::styles() {
    return _styles;
}

bool GlobalConfig::styleExists(const QString &id) {
    return _styles.contains(id);
}

Style GlobalConfig::style(const QString &id) {
    return _styles.value(id, Style());
}

Style::StyleType GlobalConfig::styleType(const QString &id) {
    return styleExists(id) ? _styles.value(id).type : Style::InvalidType;
}

QColor GlobalConfig::accentColor(const QString &id) {
    return _accentColors.value(id, QColor());
}

QColor GlobalConfig::accentColorDark(const QString &id) {
    QColor color = accentColor(id);
    return QColor(color.red() * 0.5, color.green() * 0.5, color.blue() * 0.5);
}

QMap<QString, QColor> GlobalConfig::accentColors() {
    return _accentColors;
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

void GlobalConfig::loadStyles() {
    qInfo() << "   Loading styles...";
    const QJsonArray shortcuts = loadMultiConfigResource("Styles");
    for(const QJsonValue &val : shortcuts) {
        const QJsonObject obj = val.toObject();
        Style style(obj);
        if(style.id().isEmpty())
            continue;

        _styles.insert(style.id(), style);
        qInfo().noquote() << "      - " + style.id();
    }
}

void GlobalConfig::loadAccentColors() {
    qInfo() << "   Loading accent colors...";
    const QJsonDocument doc = loadSingleConfigResource("accentColors");
    const QJsonObject object = doc.object();
    const QStringList keys = object.keys();
    for(const QString &key : keys)
        _accentColors.insert(key, QColor(object[key].toString()));
}
