#include "AppInterfaceImpl.h"

AppInterfaceImpl::AppInterfaceImpl() :
    _projectManager(new ProjectManagerImpl(this)) {
}

AppInterfaceImpl::~AppInterfaceImpl() {}

AppInterfaceImpl *AppInterfaceImpl::instance() {
    static AppInterfaceImpl instance;
    return &instance;
}

void AppInterfaceImpl::setMainWindow(MainWindow *mainWindow) {
    _mainWindow = mainWindow;
    _projectManager->setMainWindow(mainWindow);
}

void AppInterfaceImpl::openPreferencesDialog() {
    return; // TODO
}

void AppInterfaceImpl::openPlugiunsDialog() {
    return; // TODO
}

void AppInterfaceImpl::openConfigurationDialog() {
    return; // TODO
}

void AppInterfaceImpl::quit() {
    return; // TODO
}

IProjectManagerInterface *AppInterfaceImpl::projectManager() const {
    return _projectManager;
}

ProjectManagerImpl *AppInterfaceImpl::projectManagerImpl() const {
    return _projectManager;
}

IMainWindowInterface *AppInterfaceImpl::mainWindow() const {
    return nullptr; // TODO
}
