#ifndef DOCKMANAGER_H
#define DOCKMANAGER_H

#include "Global/GlobalConfigManager.h"

#include <QObject>
#include <QKeySequence>

/**
 * @brief The DockConfig struct represents the configuration for a dock.
 *
 * It includes details such as text, tooltip, description, icon, and keyboard shortcut information.
 */
struct DockConfig : public GlobalConfigItem {
public:
    /**
     * @brief Constructs a DockConfig object from a JSON object.
     * @param jsonObject The JSON object containing the dock configuration.
     * @param index The index of the dock.
     */
    DockConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    /**
     * @brief Constructs a DockConfig object with a given ID.
     * @param id The ID of the dock.
     * @param index The index of the dock.
     */
    DockConfig(const QString &id, const int &index = 0) : GlobalConfigItem(id, index) {}

    QString name; ///<The name of the dock.
    QString icon; ///<The icon of the dock.
    QKeySequence defaultKeyboardShortcut; ///<The default keyboard shortcut for the dock.
};

/**
 * @brief The DockManager class manages all available docks in the application.
 *
 * This class provides a way to manage all docks in the application. It is possible to add a dock with a dock config
 * object. The dock config object contains information about the dock like its name, icon and default shortcut.
 *
 * The DockManager also provides a way to get all available docks and to get a specific dock by its id.
 *
 * The DockManager emits the dockAdded signal whenever a new dock is added.
 *
 * The DockManager also stores all dock toggle actions. These actions are used to toggle the visibility of a dock.
 */
class DockManager : public GlobalConfigManager<DockManager, DockConfig> {
    friend class GlobalConfigManager<DockManager, DockConfig>;
    Q_OBJECT
    Q_DISABLE_COPY(DockManager)

protected:
    /**
     * @brief Constructs a DockManager object.
     * @param parent The parent QObject.
     */
    explicit DockManager(QObject *parent) : GlobalConfigManager(parent) {}

public:
    /**
     * @brief Initializes the DockManager.
     *
     * This method is called in the static constructor of the DockManager class.
     */
    static void init();
};

#endif // DOCKMANAGER_H

