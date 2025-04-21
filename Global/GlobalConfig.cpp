#include "GlobalConfig.h"

SettingsItem::SettingsItem(const QJsonObject &jsonObject) {
    id = jsonObject.value("id").toString();
    description = jsonObject.value("description").toString();

    QString typeStr = jsonObject.value("type").toString();
    if (typeStr == "group")
        isGroup = true;

    typeStr = isGroup ? jsonObject.value("content_type").toString() : typeStr;

    type = static_cast<QMetaType::Type>(QMetaType::fromName(typeStr.toUtf8()).id());

    if (isGroup) {
        groupContentType = type;
        type = QMetaType::Void;
    }

    if (!isGroup)
        if(jsonObject.contains("default"))
            defaultValue = VariantConverter::convertFromJson(jsonObject.value("default"), type);

    requiresRestart = jsonObject.value("requireRestart").toBool(false);
}


KeyboardShortcut::KeyboardShortcut(const QJsonObject &jsonObject) {
    id          = jsonObject.value("id").toString();
    description = jsonObject.value("description").toString(id);
    icon        = jsonObject.value("icon").toString();

    QJsonValue defaultValue = jsonObject.value("default");
    QStringList defaultValues;

    if(defaultValue.isArray())
        defaultValues = Global::jsonArrayToStringList(defaultValue.toArray());
    else
        defaultValues = {defaultValue.toString()};

    for(const QString & value : std::as_const(defaultValues)) {
        if(value.startsWith("standard:")) {
            QString idPart = value.mid(QString("standard:").length());
            bool ok;
            int standardID = idPart.toInt(&ok);
            if (ok) {
                QKeySequence sequence = QKeySequence(static_cast<QKeySequence::StandardKey>(standardID));
                if(!sequence.toString(QKeySequence::PortableText).isEmpty()) {
                    defaultKeySequence = sequence;
                    return;
                } else {
                    defaultKeySequence = QKeySequence();
                }
            }
        }

        QKeySequence sequence(value);
        if (!sequence.toString(QKeySequence::PortableText).isEmpty()) {
            defaultKeySequence = sequence;
            return;
        } else
            defaultKeySequence = QKeySequence();
    }
}

Style::Style(const QJsonObject &jsonObject) {
    id                 = jsonObject.value("id").toString();
    name               = jsonObject.value("name").toString(id);
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

bool Style::supportsColorScheme(const Qt::ColorScheme &colorScheme) {
    return (
        (colorScheme == Qt::ColorScheme::Unknown && lightSupport && darkSupport) ||
        (colorScheme == Qt::ColorScheme::Light && lightSupport) ||
        (colorScheme == Qt::ColorScheme::Dark && darkSupport)
    );
}

QList<Qt::ColorScheme> Style::supportedColorSchemes() {
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

void GlobalConfig::initLanguages() {
    qInfo() << "Loading global configuration (1/2)...";

    loadSupportedLanguages();
}

void GlobalConfig::init() {
    qInfo() << "Loading global configuration (2/2)...";

    loadSettingsItems();
    loadFolderLocations();
    loadKeyboardShortcuts();
    loadStyles();
    loadAccentColors();
}

QList<SettingsItem> GlobalConfig::settingsItems() {
    return _settingsItems.values();
}

bool GlobalConfig::settingsItemExists(const QString &id) {
    return _settingsItems.contains(id);
}

SettingsItem GlobalConfig::settingsItem(const QString &id) {
    return settingsItemExists(id) ? _settingsItems.value(id) : SettingsItem();
}

QMetaType::Type GlobalConfig::settingsItemDataType(const QString &id) {
    return settingsItemExists(id) ? settingsItem(id).type : QMetaType::Void;
}

QSet<QString> GlobalConfig::restartRequiredSettings() {
    return _restartRequiredSettings;
}

bool GlobalConfig::settingRequiresRestart(const QString &id) {
    return _restartRequiredSettings.contains(id);
}

void GlobalConfig::registerNewSettingsItem(const SettingsItem &item) {
    if(item.id.isEmpty())
        return;

    if(item.requiresRestart)
        _restartRequiredSettings << item.id;

    _settingsItems.insert(item.id, item);
    qInfo().noquote() << "Registered new settings item: " + item.id;
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

QStringList GlobalConfig::folderLocationIDs() {
    return _folderLocations.keys();
}

QList<KeyboardShortcut> GlobalConfig::keyboardShortcuts() {
    return _keyboardShortcuts.values();
}

bool GlobalConfig::keyboardShortcutExists(const QString &id) {
    return _keyboardShortcuts.contains(id);
}

KeyboardShortcut GlobalConfig::keyboardShortcut(const QString &id) {
    return keyboardShortcutExists(id) ? _keyboardShortcuts.value(id) : KeyboardShortcut();
}

QKeySequence GlobalConfig::keyboardShortcutDefaultKeySequence(const QString &id) {
    return keyboardShortcutExists(id) ? _keyboardShortcuts.value(id).defaultKeySequence : QKeySequence();
}

bool GlobalConfig::keyboardShortcutIsDefault(const QString &id, const QKeySequence &sequence) {
    if(!keyboardShortcutExists(id))
        return false;

    return keyboardShortcut(id).defaultKeySequence == sequence;
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

void GlobalConfig::loadSettingsItems() {
    qInfo() << "   Loading settings items...";
    const QJsonArray items = loadMultiConfigResource("Settings");
    for(const QJsonValue &val : items) {
        const QJsonObject obj = val.toObject();
        SettingsItem item(obj);
        if(item.id.isEmpty())
            continue;

        if(item.requiresRestart)
            _restartRequiredSettings << item.id;

        _settingsItems.insert(item.id, item);
        qInfo().noquote() << "      - " + item.id;
    }
}

void GlobalConfig::loadSupportedLanguages() {
    qInfo() << "   Loading supported languages...";
    const QJsonArray languages = loadMultiConfigResource("Languages");
    for(const QJsonValue &val : languages) {
        const QString lang = val.toString();
        const QLocale locale(lang);
        _supportedLanguages << locale.language();
        qInfo().noquote() << "      - " + lang;
    }
}

void GlobalConfig::loadFolderLocations() {
    qInfo() << "   Loading folder locations...";
    const QJsonArray locations = loadMultiConfigResource("Locations");
    for(const QJsonValue &val : std::as_const(locations)) {
        const QJsonObject obj = val.toObject();

        const QString id     = obj.value("id").toString();
              QString name   = obj.value("name").toString();
        const QString icon   = obj.value("icon").toString();
        bool multiple        = obj.value("multiple").toBool();
        bool requiresRestart = obj.value("changeRequiresRestart").toBool();

        // fallback if no name is given
        if(name.isEmpty())
            name = id;

        const FolderLocation location(id, name, icon, multiple);

        _folderLocations.insert(id, location);
        qInfo().noquote() << "      - " + id;

        SettingsItem item;
        item.id              = "locations/" + id;
        item.type            = QMetaType::QStringList;
        item.description     = name;
        item.requiresRestart = requiresRestart;

        QStringList defaultValue;

        if(id == "base.projectFilesDefault")
            defaultValue = {QDir::homePath() + "/ScheduleMaster/Projects"};
        else if(id == "base.logfile")
            defaultValue = {defaultLogfileLocation()};
        else if(id == "base.plugins")
            defaultValue = {QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/plugins", QCoreApplication::applicationDirPath() + "/plugins"};
        else
            defaultValue = QStringList();

        item.defaultValue = defaultValue;

        registerNewSettingsItem(item);
    }
}

QString GlobalConfig::defaultLogfileLocation() {
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/logs";
}

void GlobalConfig::loadKeyboardShortcuts() {
    qInfo() << "   Loading keyboard shortcuts...";
    const QJsonArray shortcuts = loadMultiConfigResource("KeyboardShortcuts");
    for(const QJsonValue &val : shortcuts) {
        const QJsonObject obj = val.toObject();
        KeyboardShortcut shortcut(obj);
        if(shortcut.id.isEmpty())
            continue;

        _keyboardShortcuts.insert(shortcut.id, shortcut);
        qInfo().noquote() << "      - " + shortcut.id;

        SettingsItem item;
        item.id           = "keyboardShortcuts/" + shortcut.id;
        item.type         = QMetaType::QKeySequence;
        item.description  = shortcut.description;
        item.defaultValue = shortcut.defaultKeySequence;
        registerNewSettingsItem(item);
    }
}

void GlobalConfig::loadStyles() {
    qInfo() << "   Loading styles...";
    const QJsonArray shortcuts = loadMultiConfigResource("Styles");
    for(const QJsonValue &val : shortcuts) {
        const QJsonObject obj = val.toObject();
        Style style(obj);
        if(style.id.isEmpty())
            continue;

        _styles.insert(style.id, style);
        qInfo().noquote() << "      - " + style.id;
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

