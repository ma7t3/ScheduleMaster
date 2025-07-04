#include "FolderLocationManager.h"

#include "Global/SettingsManager.h"

#include <QApplication>
#include <QStandardPaths>

FolderLocationConfig::FolderLocationConfig(const QJsonObject &jsonObject, const int &index) : GlobalConfigItem(jsonObject, index) {
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
    SettingsManager::registerNewSettingsItem(item);
}

QString FolderLocationConfig::resolvePathPlaceholders(QString path) {
    path.replace("${HOME}",           QDir::homePath());
    path.replace("${APP_LOCAL_DATA}", QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    path.replace("${APP_DIR}",        QCoreApplication::applicationDirPath());
    return path;
}

void FolderLocationManager::init() {
    GlobalConfigManager::init();
    loadItems("Locations");

    connect(SettingsManager::instance(), &SettingsManager::valueChanged, instance(), [](const QString &settingID, const QVariant &value) {
        if(!settingID.startsWith("locations/"))
            return;

        QString locationID = settingID;
        locationID.remove("locations/");
        emit instance()->currentFolderLocationPathsChanged(locationID, value.toStringList());
    });
}

QMap<QString, QStringList> FolderLocationManager::currentFolderLocations() {
    QMap<QString, QStringList> data;

    const QStringList keys = SettingsManager::keysInGroup("locations");

    const QStringList standardKeys = itemIDs();
    for(const QString &key : standardKeys) {
        if(!keys.contains(key)) {
            data[key] = SettingsManager::item("locations/" + key).defaultValue.toStringList();
            SettingsManager::setValue("locations/" + key, data[key]);
        }
    }

    for(const QString &id : keys)
        data[id] = currentFolderLocationPaths(id);

    return data;
}

QStringList FolderLocationManager::currentFolderLocationPaths(const QString &folderLocationID) {
    QStringList values = SettingsManager::value("locations/" + folderLocationID).toStringList();
    if(values.empty()) {
        values = SettingsManager::item("locations/" + folderLocationID).defaultValue.toStringList();
        SettingsManager::setValue("locations/" + folderLocationID, values);
    }

    if(values.isEmpty())
        values << "";

    return values;
}

void FolderLocationManager::setCurrentFolderLocationPaths(const QString &folderLocationID, const QStringList &paths) {
    SettingsManager::setValue("locations/" + folderLocationID, paths);
}

void FolderLocationManager::setCurrentFolderLocations(const QMap<QString, QStringList> &folderLocations) {
    const QStringList locationKeys = folderLocations.keys();
    for(const QString &key : locationKeys)
        setCurrentFolderLocationPaths(key, folderLocations.value(key));
}
