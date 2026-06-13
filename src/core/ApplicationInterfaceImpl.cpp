#include "ApplicationInterfaceImpl.h"

#include "core/CrashDetectorImpl.h"
#include "core/LoggerImpl.h"
#include "core/FolderLocationServiceImpl.h"
#include "core/SettingsServiceImpl.h"
#include "core/LanguageServiceImpl.h"
#include "core/LastUsedFilesServiceImpl.h"
#include "core/IconServiceImpl.h"
#include "core/AppearanceServiceImpl.h"
#include "core/ActionServiceImpl.h"
#include "core/DockServiceImpl.h"
#include "core/WorkspaceServiceImpl.h"

namespace ScheduleMaster::Core {

ApplicationInterfaceImpl::ApplicationInterfaceImpl(QObject *parent) : QObject(parent) {
    _self = this;

    _settingsService = new SettingsServiceImpl(this);
    _crashDetector = new CrashDetectorImpl(this);
    _folderLocationService = new FolderLocationServiceImpl(this);
    _logger = new LoggerImpl(this);

    qInfo() << "Starting ScheduleMaster...";
    _settingsService->initRepository();
    _folderLocationService->initRepository();


    _languageService      = new LanguageServiceImpl(this);
    _iconService          = new IconServiceImpl(this);
    _lastUsedFilesService = new LastUsedFilesServiceImpl(this);
    _appearanceService    = new AppearanceServiceImpl(this);
    _actionService        = new ActionServiceImpl(this);
    _dockService          = new DockServiceImpl(this);

    _mainWindow           = new MainWindow;
    _workspaceService     = new WorkspaceServiceImpl(_mainWindow, this);
    _workspaceService->setWorkspacesMenu(_mainWindow->workspacesMenu());
    _workspaceService->setWorkspacesToolbar(_mainWindow->workspacesToolbar());

    _dockService->loadStandardDocks();

    bool ok = _mainWindow->restoreGeometry(_settingsService->value("general.mainWindowGeometry").toByteArray());
    if(!ok)
        _mainWindow->showMaximized();
    else
        _mainWindow->show();
}

ApplicationInterfaceImpl::~ApplicationInterfaceImpl() {
    _settingsService->setValue("general.mainWindowGeometry", _mainWindow->saveGeometry());
    CrashDetectorImpl::instance()->shutdown();
    delete _mainWindow;
}

ApplicationInterfaceImpl *ApplicationInterfaceImpl::instance() {
    return _self;
}

ICrashDetector *ApplicationInterfaceImpl::crashDetector() const {
    return _crashDetector;
}

ILogger *ApplicationInterfaceImpl::logger() const {
    return _logger;
}

IFolderLocationService *ApplicationInterfaceImpl::folderLocationService() const {
    return _folderLocationService;
}

ISettingsService *ApplicationInterfaceImpl::settingsService() const {
    return _settingsService;
}

ILanguageService *ApplicationInterfaceImpl::languageService() const {
    return _languageService;
}

ILastUsedFilesService *ApplicationInterfaceImpl::lastUsedFilesService() const {
    return _lastUsedFilesService;
}

IIconService *ApplicationInterfaceImpl::iconService() const {
    return _iconService;
}

IAppearanceService *ApplicationInterfaceImpl::appearanceService() const {
    return _appearanceService;
}

IActionService *ApplicationInterfaceImpl::actionService() const {
    return _actionService;
}

IDockService *ApplicationInterfaceImpl::dockService() const {
    return _dockService;
}

IWorkspaceService *ApplicationInterfaceImpl::workspaceService() const {
    return _workspaceService;
}

IMainWindow *ApplicationInterfaceImpl::mainWindow() const {
    return _mainWindow;
}

}
