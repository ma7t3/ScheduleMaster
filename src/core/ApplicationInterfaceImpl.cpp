#include "ApplicationInterfaceImpl.h"

#include "src/core/CrashDetectorImpl.h"
#include "src/core/LoggerImpl.h"
#include "src/core/FolderLocationServiceImpl.h"
#include "src/core/SettingsServiceImpl.h"
#include "src/core/LanguageServiceImpl.h"
#include "src/core/LastUsedFilesServiceImpl.h"
#include "src/core/IconServiceImpl.h"

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

    _languageService = new LanguageServiceImpl(this);
    _iconService = new IconServiceImpl(this);
    _lastUsedFilesService = new LastUsedFilesServiceImpl(this);
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
}
