#ifndef SCHEDULEMASTER_H
#define SCHEDULEMASTER_H

#include "ScheduleMaster_global.h"

namespace ScheduleMaster {

class ICrashDetector;
class ILogger;
class IFolderLocationService;
class ISettingsService;
class ILanguageService;
class ILastUsedFilesService;
class IIconService;
class IAppearanceService;
class IActionService;
class IDockService;
class IWorkspaceService;

class IMainWindow;

class SCHEDULEMASTERINTERFACE_EXPORT IApplicationInterface {
public:
    IApplicationInterface();
    virtual ~IApplicationInterface() = default;
    static IApplicationInterface *instance();

    virtual ICrashDetector *crashDetector() const = 0;
    virtual ILogger *logger() const = 0;
    virtual IFolderLocationService *folderLocationService() const = 0;
    virtual ISettingsService *settingsService() const = 0;
    virtual ILanguageService *languageService() const = 0;
    virtual ILastUsedFilesService *lastUsedFilesService() const = 0;
    virtual IIconService *iconService() const = 0;
    virtual IAppearanceService *appearanceService() const = 0;
    virtual IActionService *actionService() const = 0;
    virtual IDockService *dockService() const = 0;
    virtual IWorkspaceService *workspaceService() const = 0;

    virtual IMainWindow *mainWindow() const = 0;

protected:
    static IApplicationInterface *_self;
};

}

#endif // SCHEDULEMASTER_H
