#include "FolderLocationsHandler.h"

FolderLocation::FolderLocation(const QJsonObject &jsonObject) : GlobalConfigItem(jsonObject) {
    name            = jsonObject.value("name").toString(id());
    icon            = jsonObject.value("icon").toString();
    multiple        = jsonObject.value("multiple").toBool();
    requiresRestart = jsonObject.value("changeRequiresRestart").toBool();
    defaultPaths    = jsonObject.value("default").toVariant().toStringList();

    for(int i = 0; i < defaultPaths.size(); i++)
        defaultPaths[i] = resolvePathPlaceholders(defaultPaths[i]);

    if(!multiple)
        defaultPaths = defaultPaths.isEmpty() ? QStringList{""} : QStringList{defaultPaths.first()};

    SettingsItem item("locations/" + id());
    item.type            = QMetaType::QStringList;
    item.description     = name;
    item.requiresRestart = requiresRestart;
    item.defaultValue    = defaultPaths;
    GlobalConfig::registerNewSettingsItem(item);
}

QString FolderLocation::resolvePathPlaceholders(QString path) {
    path.replace("${HOME}",           QDir::homePath());
    path.replace("${APP_LOCAL_DATA}", QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    path.replace("${APP_DIR}",        QCoreApplication::applicationDirPath());
    return path;
}

FolderLocation::FolderLocation(const QString &id) : GlobalConfigItem(id) {}

FolderLocationsHandler::FolderLocationsHandler(QObject *parent) : GlobalConfigHandler(parent) {
    loadItems("Locations");
}

FolderLocationsHandler *FolderLocationsHandler::instance() {
    static FolderLocationsHandler instance(nullptr);
    return &instance;
}

void FolderLocationsHandler::init() {
    instance();
}
