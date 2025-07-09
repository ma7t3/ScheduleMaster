#include "AppInterfaceImpl.h"

AppInterfaceImpl *AppInterfaceImpl::instance() {
    static AppInterfaceImpl instance;
    return &instance;
}

void AppInterfaceImpl::openPreferencesDialog() {
    return; // TODO
}

void AppInterfaceImpl::openConfigurationDialog() {
    return; // TODO
}

IProjectManagerInterface *AppInterfaceImpl::projectManager() {
    return nullptr; // TODO
}

IMainWindowInterface *AppInterfaceImpl::mainWindow() {
    return nullptr; // TODO
}
