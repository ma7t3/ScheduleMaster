#include "FolderLocationServiceImpl.h"

#include "Global/SettingsManager.h"

#include <QCoreApplication>
#include <QStandardPaths>

namespace ScheduleMaster::Core {

FolderLocationServiceImpl::FolderLocationServiceImpl(QObject *parent) : GlobalConfigServiceCRTP(parent, "Locations") {
    connect(SettingsManager::instance(), &SettingsManager::valueChanged, this, [this](const QString &settingID, const QVariant &value) {
        if(!settingID.startsWith("locations/"))
            return;

        QString locationID = settingID;
        locationID.remove("locations/");
        emit currentFolderLocationPathsChanged(locationID, value.toStringList());
    });
}

QMap<QString, QStringList> FolderLocationServiceImpl::currentFolderLocations() const {
    QMap<QString, QStringList> data;

    const QStringList keys = SettingsManager::keysInGroup("locations");

    const QStringList standardKeys = repository()->itemIDs();
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

QStringList FolderLocationServiceImpl::currentFolderLocationPaths(const QString &folderLocationID) const {
    QStringList values = SettingsManager::value("locations/" + folderLocationID).toStringList();
    if(values.empty()) {
        values = SettingsManager::item("locations/" + folderLocationID).defaultValue.toStringList();
        SettingsManager::setValue("locations/" + folderLocationID, values);
    }

    if(values.isEmpty())
        values << "";

    return values;
}

void FolderLocationServiceImpl::setCurrentFolderLocationPaths(const QString &folderLocationID,
                                                          const QStringList &paths) {
    SettingsManager::setValue("locations/" + folderLocationID, paths);
}

void FolderLocationServiceImpl::setCurrentFolderLocations(
    const QMap<QString, QStringList> &folderLocations) {
    const QStringList locationKeys = folderLocations.keys();
    for(const QString &key : locationKeys)
        setCurrentFolderLocationPaths(key, folderLocations.value(key));
}

bool FolderLocationServiceImpl::folderLocationExists(const QString &id) const {
    return repository()->itemExists(id);
}

FolderLocationConfig FolderLocationServiceImpl::folderLocation(const QString &id) const {
    return repository()->item(id);
}

QList<FolderLocationConfig> FolderLocationServiceImpl::folderLocations() const {
    return repository()->sortedItems();
}

bool FolderLocationServiceImpl::registerFolderLocations(FolderLocationConfig folderLocation) {
    return repository()->addItem(folderLocation);
}

}
