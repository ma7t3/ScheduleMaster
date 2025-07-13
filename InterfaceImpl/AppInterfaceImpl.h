#ifndef APPINTERFACEIMPL_H
#define APPINTERFACEIMPL_H

#include <ScheduleMasterInterface/IScheduleMasterInterface.h>

#include "ProjectManagerImpl.h"

#define appInterface (AppInterfaceImpl::instance())

class AppInterfaceImpl : public IScheduleMasterInterface {
    Q_OBJECT

protected:
    AppInterfaceImpl();
    ~AppInterfaceImpl() override;

public:
    static AppInterfaceImpl *instance();

    void setMainWindow(MainWindow *mainWindow);

    void openPreferencesDialog() override;
    void openPlugiunsDialog() override;
    void openConfigurationDialog() override;

    void quit() override;

    IProjectManagerInterface *projectManager() const override;
    ProjectManagerImpl *projectManagerImpl() const;

    IMainWindowInterface *mainWindow() const override;

protected:
    ProjectManagerImpl *_projectManager;

    MainWindow *_mainWindow;
};

#endif // APPINTERFACEIMPL_H
