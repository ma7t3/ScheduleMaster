#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QObject>
#include <QIcon>
#include <QAction>
#include <QJsonObject>
#include <QMainWindow>

/**
 * @brief The Workspace class one UI workspace.
 *
 * Since the Schedule Master UI is made out of DockWidgets, there are inifite possilbe ways to arrange these docks in ways the user prefers.
 * To make it easier for the user to switch between different dock arrangements, it is possible to define different workspaces.
 * One workspace always consists of a set of docks and the way they are arranged.
 *
 * For identification purposes, every workspace has a name and an icon that is displayed in the UI.
 *
 * Every workspace also owns a QAction that is used to switch to the workspace.
 *
 * All existing workspaces are stored inside the WorkspaceHandler class.
 */
class Workspace : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Creates a new Workspace with the given parent.
     *
     * A associated QAction is created automatically.
     * @param parent The QObject-parent
     */
    explicit Workspace(const QString &id, QObject *parent);

    /**
     * @brief Creates a new Workspace with the given name, icon and parent.
     *
     * A associated QAction is created automatically.
     * @param name The name of the workspace
     * @param icon The icon of the workspace
     * @param parent The QObject-parent
     */
    Workspace(const QString &id, const QString &name, const QIcon &icon, QObject *parent);

    /**
     * @brief Creates a new Workspace based on the given QAction.
     *
     * It will take the QAction's text as name and the QAction's icon as icon.
     * @param action The QAction to create the workspace from
     * @param parent The QObject-parent
     */
    Workspace(const QString &id, QAction *action, QObject *parent);

    /**
     * @brief Creates a new Workspace based on the given QJsonObject.
     * @param json The json object
     * @param parent The QObject-parent
     */
    Workspace(const QJsonObject &json, QObject *parent);

    /// Returns whether this workspace is currently active or not.
    bool active() const;

    /**
     * @brief Returns the id of the workspace
     * @return The workspace's id
     */
    QString id() const;

    /**
     * @brief Returns the name of the workspace.
     *
     * See also setName().
     * @return The workspace's name
     */
    QString name() const;

    /**
     * @brief Sets the name of the workspace.
     *
     * See also name().
     * @param newName The new name to set
     */
    void setName(const QString &newName);

    /**
     * @brief Returns the icon of the workspace
     *
     * See also setIcon().
     * @return The workspace's icon
     */
    QIcon icon() const;

    /**
     * @brief Sets the icon of the workspace.
     *
     * See also icon().
     * @param newIcon The new icon to set
     */
    void setIcon(const QIcon &newIcon);

    /**
     * @brief Returns the QAction that is associated with this workspace.
     *
     * This cannot be nullptr and always points to a valid QAction.
     * @return The corresponding QAction
     */
    QAction *action() const;

    /**
     * @brief Returns the state of the main window when the workspace was deactivated last
     * @return The QMainWindow state
     */
    QByteArray lastWindowState() const;

public slots:

    /**
     * @brief Actives the workspace.
     *
     * This only marks the workspace as activated and emits the activated() signal but it doesn't change anything to the main windows layout.
     * You need to call apply() after.
     *
     * See also deactivate().
     */
    void activate();

    /**
     * @brief Deactivates the workspace.
     *
     * You should only call this method if you make sure that another workspace is activated. Consider just calling activate() on another workspace.
     *
     * See also activate().
     */
    void deactivate();

    /**
     * @brief Applies the workspace to the mainWindow.
     */
    void apply();

    /**
     * @brief Restores the workspace's default layout and discards the _lastWindowState.
     */
    void restore();

protected:
    /// Sets up the QAction.
    void setupAction();

    /// Determins the pointer to the applications QMainWindow.
    QMainWindow *mainWindow();

signals:
    /**
     * @brief This signals is emited when the workspace is activated. This happens when activate() is called or the QAction is triggered.
     *
     * @param workspace The workspace that was activated ("this").
     */
    void activated(Workspace *workspace);

    /**
     * @brief This signals is emited when restore() is called.
     *
     * @param workspace The workspace that was activated ("this").
     */
    void restored(Workspace *workspace);

private:
    /// The workspace's id
    QString _id;

    /// The workspace's name (is displayed in the UI)
    QString _name;

    /// The workspace's icon (is displayed in the UI)
    QIcon _icon;

    /// The workspace's associated QAction that is used to activate the workspace
    QAction *_action;

    /// The last state of the MainWindow when the workspace was deactivated last
    QByteArray _lastWindowState;
};

#endif // WORKSPACE_H
