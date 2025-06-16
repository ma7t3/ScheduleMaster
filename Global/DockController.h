#ifndef DOCKCONTROLLER_H
#define DOCKCONTROLLER_H

#include "Global/DockManager.h"

#include <QObject>

class QDockWidget;
class QAction;

/**
 * @brief The DockController class handles all available docks in the application.
 *
 * This class provides a way to manage all docks in the application. It is possible to add a dock with a content widget
 * and a dock config object. The dock config object contains information about the dock like its name, icon and default
 * shortcut.
 *
 * The DockController also provides a way to get all available docks and to get a specific dock by its id.
 *
 * The DockController emits the dockAdded signal whenever a new dock is added.
 *
 * The DockController also stores all dock toggle actions. These actions are used to toggle the visibility of a dock.
 */
class DockController : public QObject {
    Q_OBJECT
public:
    explicit DockController(QObject *parent = nullptr);

    /**
     * @brief Loads all built-in standard docs
     */
    void loadStandardDocks();

    /**
     * @brief Adds a dock to the application.
     * @param dock The dock config for the new dock.
     * @param contentWidget The content widget for the new dock.
     */
    void addDock(const DockConfig &dock, QWidget *contentWidget);

    /**
     * @brief Gets all available docks.
     * @return A map containing all available docks with their id as key.
     */
    static QMap<QString, QDockWidget *> docks();

    /**
     * @brief Gets all available docks as a list.
     * @return A list containing all available docks ordered by their index.
     */
    static QList<QDockWidget *> dockList();

    /**
     * @brief Gets a specific dock by its id.
     * @param id The id of the dock to get.
     * @return The dock with the given id or nullptr if no dock with the given id exists.
     */
    static QDockWidget *dock(const QString &id);

signals:
    /**
     * @brief Emitted whenever a new dock is added.
     * @param id The id of the new dock.
     * @param dockWidget The new dock widget.
     * @param toggleViewAction The toggle view action for the new dock.
     */
    void dockAdded(const QString &id, QDockWidget *dockWidget, QAction *toggleViewAction);

private:
    /**
     * @brief A map containing all available docks with their id as key.
     *
     * This map is used to store all available docks in the application. The id of the dock is used as the key and the dock
     * widget as the value.
     */
    static inline QMap<QString, QDockWidget *> _docks;

    /**
     * @brief A map containing all dock toggle actions with the id of the dock as key.
     *
     * This map is used to store all dock toggle actions. The id of the dock is used as the key and the toggle action as the
     * value.
     */
    QMap<QString, QAction *> _dockToggleActions;
};

#endif // DOCKCONTROLLER_H

