#ifndef FOLDERLOCATIONMANAGER_H
#define FOLDERLOCATIONMANAGER_H

#include "Global/GlobalConfigManager.h"

#include <QObject>

/**
 * @brief The FolderLocationConfig struct represents a folder location configuration.
 *
 * This struct is used in the FolderLocationManager to manage folder locations and their configurations.
 */
struct FolderLocationConfig : public GlobalConfigItem {
public:
    /**
     * @brief Constructs a FolderLocationConfig object from a JSON object.
     * @param jsonObject The JSON object to construct the FolderLocationConfig from.
     * @param index The index of the FolderLocationConfig in the FolderLocationManager.
     */
    FolderLocationConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    /**
     * @brief Constructs a FolderLocationConfig object with the given ID.
     * @param id The ID of the FolderLocationConfig.
     * @param index The index of the FolderLocationConfig in the FolderLocationManager.
     */
    FolderLocationConfig(const QString &id, const int &index = 0) : GlobalConfigItem(id, index) {}

    QString name; ///< The name of the folder location.
    QString icon; ///< The icon of the folder location.
    QStringList defaultPaths; ///< The default paths of the folder location.
    bool multiple; ///< Whether the folder location can have multiple paths.
    bool requiresRestart; ///< Whether the folder location requires a restart of the application to apply changes.

protected:
    /**
     * @brief Resolves path placeholders in the given path.
     * @param path The path to resolve placeholders for.
     * @return The path with placeholders resolved.
     */
    static QString resolvePathPlaceholders(QString path);
};

/**
 * @brief The FolderLocationManager class manages folder locations and their configurations.
 *
 * It provides functionalities to initialize folder locations, manage the current folder locations,
 * and import/export folder location configurations.
 *
 * The FolderLocationManager is a singleton class, meaning that it can be accessed from anywhere in the application.
 */
class FolderLocationManager : public GlobalConfigManager<FolderLocationManager, FolderLocationConfig> {
    friend class GlobalConfigManager<FolderLocationManager, FolderLocationConfig>;
    Q_OBJECT
protected:
    Q_DISABLE_COPY(FolderLocationManager)

    /**
     * @brief Constructs a FolderLocationManager object.
     * @param parent The parent QObject.
     */
    explicit FolderLocationManager(QObject *parent) : GlobalConfigManager(parent) {}

public:
    /**
     * @brief Initializes the FolderLocationManager.
     */
    static void init();

    /**
     * @brief Retrieves the current folder locations.
     * @return A map of folder location IDs to their current paths.
     */
    static QMap<QString, QStringList> currentFolderLocations();
    /**
     * @brief Retrieves the current paths of a given folder location.
     * @param folderLocationID The ID of the folder location.
     * @return The current paths of the folder location.
     */
    static QStringList currentFolderLocationPaths(const QString &folderLocationID);

    /**
     * @brief Sets the current paths of a given folder location.
     * @param folderLocationID The ID of the folder location.
     * @param paths The paths to set for the folder location.
     */
    static void setCurrentFolderLocationPaths(const QString &folderLocationID, const QStringList &paths);
    /**
     * @brief Sets the current folder locations.
     * @param folderLocations A map of folder location IDs to their current paths.
     */
    static void setCurrentFolderLocations(const QMap<QString, QStringList> &folderLocations);

signals:
    /**
     * @brief Emitted when the current paths of a folder location have changed.
     * @param folderLocationID The ID of the folder location.
     * @param paths The new paths of the folder location.
     */
    void currentFolderLocationPathsChanged(const QString &folderLocationID, const QStringList &paths);
};

#endif // FOLDERLOCATIONMANAGER_H
