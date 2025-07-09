#ifndef ISCHEDULEMASTERINTERFACE_H
#define ISCHEDULEMASTERINTERFACE_H

#include <QAction>
#include <QToolBar>

#include "ScheduleMasterInterface_global.h"

class SCHEDULEMASTERINTERFACE_EXPORT IProjectDataInterface : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(IProjectDataInterface)

protected:
    IProjectDataInterface() = default;
    virtual ~IProjectDataInterface() override = default;

public:
};

class SCHEDULEMASTERINTERFACE_EXPORT IMainWindowInterface :public  QObject {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(IMainWindowInterface)

protected:
    IMainWindowInterface() = default;
    virtual ~IMainWindowInterface() override = default;

public:
    enum StandardMenu {
        FileMenu,
        EditMenu,
        ViewMenu
    };

    enum StandardToolBar {
        MiscellaneousToolBar
    };

    virtual void close() = 0;

    virtual void addActionToStandardMenu(const StandardMenu &menu, QAction *action) = 0;
    virtual void addActionToDockMenu(const QString &dockID, QAction *action) = 0;
    virtual void addActionToStandardToolBar(const StandardToolBar &toolBar, QAction *action) = 0;
    virtual void addToolBar(QToolBar *toolBar) = 0;

    virtual QMainWindow *qMainWindow() = 0;

signals:
    void aboutToClose();
    void closed();
};

class SCHEDULEMASTERINTERFACE_EXPORT IProjectManagerInterface : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(IProjectManagerInterface)

protected:
    IProjectManagerInterface() = default;
    virtual ~IProjectManagerInterface() override = default;

public:
    virtual IProjectDataInterface *project() = 0;

    virtual void newProject() = 0;
    virtual void openProject() = 0;
    virtual void openProjectFromLocation(const QString &url) = 0;
    virtual void saveProject() = 0;
    virtual void saveProjectAs() = 0;
    virtual void saveProjectToLocation(const QString &url) = 0;
    virtual void closeProject() = 0;
    virtual QStringList lastUsedProjectFiles() = 0;

signals:
    void projectAboutToClose(const QString &url);
    void projectClosed(const QString &url);
    void projectAboutToOpen(const QString &url);
    void projectOpened(const QString &url);
    void projectAboutToSave(const QString &url);
    void projectSaved(const QString &url);
    void lastUsedProjectFilesChanged(const QStringList &list);
};

class SCHEDULEMASTERINTERFACE_EXPORT IScheduleMasterInterface : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(IScheduleMasterInterface)

protected:
    IScheduleMasterInterface() = default;
    virtual ~IScheduleMasterInterface() override = default;

public:
    virtual void openPreferencesDialog() = 0;
    virtual void openConfigurationDialog() = 0;

    virtual IProjectManagerInterface *projectManager() = 0;
    virtual IMainWindowInterface *mainWindow() = 0;
};

#endif // ISCHEDULEMASTERINTERFACE_H
