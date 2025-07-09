#ifndef APPINTERFACEIMPL_H
#define APPINTERFACEIMPL_H

#include <IScheduleMasterInterface.h>

#define appInterface (AppInterfaceImpl::instance())

class AppInterfaceImpl : public IScheduleMasterInterface {
    Q_OBJECT

protected:
    AppInterfaceImpl() = default;
    virtual ~AppInterfaceImpl() override = default;

public:
    static AppInterfaceImpl *instance();

    virtual void openPreferencesDialog() override;
    virtual void openConfigurationDialog() override;

    virtual IProjectManagerInterface *projectManager() override;
    virtual IMainWindowInterface *mainWindow() override;

};

#endif // APPINTERFACEIMPL_H
