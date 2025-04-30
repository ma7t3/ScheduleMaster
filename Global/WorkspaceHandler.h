#ifndef WORKSPACEHANDLER_H
#define WORKSPACEHANDLER_H

#include <QObject>
#include <QMenu>
#include <QToolBar>
#include <QPointer>
#include <QDockWidget>
#include "Global/Workspace.h"

#include "Global/GlobalConfig.h"

#include "Global/ActionShortcutMapper.h"

/**
 * @brief The WorkspaceHandler class is a manager for all existing workspaces (see Workspace).
 *
 * It provides a way to add, remove and access workspaces as well as generates the built-in standard workspaces.
 *
 * It also handles the communication between workspaces (e.g. when a workspace is activated, another workspaces is being deactivated automatically).
 *
 * WorkspaceHandler also provides a connection to a QMenu and QToolBar to display the workspaces in the UI allowing the user to switch workspaces.
 */
class WorkspaceHandler : public QObject {
    Q_OBJECT
public:

    /**
     * @brief Creates a new WorkspaceHandler with the given parent.
     * @param parent The QObject-parent
     */
    explicit WorkspaceHandler(QObject *parent);

    /**
     * @brief Returns a list of all available workspaces (standard and custom ones)
     * @return The workspace list
     */
    QList<Workspace *> workspaces();

    /**
     * @brief Returns a pointer to the workspace with the given ID or nullptr if the workspace does not exist.
     * @param id The workspace ID
     * @return The workspace pointer or nullptr if the workspace does not exist
     */
    Workspace *workspace(const QString &id);

    /**
     * @brief Adds a new workspace to the handler.
     *
     * The handler will take QObject-ownership of the workspace.
     *
     * The workspace will not be added if workspace is nullptr.
     * @param workspace The workspace pointer to add.
     */
    void addWorkspace(Workspace *workspace);

    /**
     * @brief Adds multiple workspaces to the handler.
     *
     * This method does nothing different than calling addWorkspace() for every single list element.
     * @param The list of workspaces to add
     */
    void addWorkspaces(QList<Workspace *> workspaces);

    /**
     * @brief Removes the given workspace from the handler.
     *
     * The handler will still keep QObject-ownership of the workspace.
     *
     * If the workspace is part of the standard workspaces, it will not be removed.
     * If the workspace is not part of the handler, this method does nothing as well.
     * @param The workspace to remove
     */
    void removeWorkspace(Workspace *workspace);

    /**
     * @brief sets the QMenu to display the workspaces in.
     *
     * It is safe to set a nullptr to remove the menu. The handler will not longer try to update any menu.
     *
     * The connection to the menu is also automatically disconnected when the menu is destroyed.
     *
     * The handler will not take ownership of the menu.
     * @param newMenu The menu to set
     */
    void setWorkspacesMenu(QMenu *newMenu);

    /**
     * @brief sets the QToolBar to display the workspaces in.
     *
     * It is safe to set a nullptr to remove the toolbar. The handler will not longer try to update any toolbar.
     *
     * The connection to the toolbar is also automatically disconnected when the menu is destroyed.
     *
     * The handler will not take ownership of the toolbar.
     * @param newToolBar The toolbar to set
     */
    void setWorkspacesToolbar(QToolBar *newToolBar);

    /**
     * @brief Returns a pointer to the currently active workspace.
     *
     * This may be nullptr if there is no active workspace (e.g. before the initialisation or if no workspaces do exist).
     */
    Workspace *currentWorkspace();

protected:
    /**
     * @brief Loads the workspaces from the global configuration
     */
    void loadWorkspaces();

    /**
     * @brief Updates the linked QMenu whenever the workspace list changes.
     *
     * This method does nothing if there is no linked menu.
     *
     * See also setWorkspacesMenu().
     */
    void updateWorkspacesMenu();

    /**
     * @brief Updates the linked QToolBar whenever the workspace list changes.
     *
     * This method does nothing if there is no linked toolbar.
     *
     * See also setWorkspacesToolbar().
     */
    void updateWorkspacesToolbar();

protected slots:
    /**
     * @brief This function is called whenever a workspace is activated.
     *
     * It deactivates all other workspaces.
     * @param workspace The workspace that was activated
     */
    void onWorkspaceActivated(Workspace *workspace);

    /**
     * @brief restoreCurrentWorkspace
     */
    void restoreCurrentWorkspace();

signals:

private:
    /// The map of all registered workspaces
    QList<Workspace *> _workspaces;

    /// The linked QMenu to display the workspaces. This can also be nullptr.
    QPointer<QMenu> _workspacesMenu;

    /// The linked QToolBar to display the workspaces. This can also be nullptr.
    QPointer<QToolBar> _workspacesToolbar;

    /// The map of all available docks
    QMap<QString, QDockWidget *> _docks;

    /// The menu action that is used to restore the current workspace's default layout
    QAction *_restoreLayoutAction;
};

#endif // WORKSPACEHANDLER_H
