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

class SCHEDULEMASTERINTERFACE_EXPORT IApplicationInterface {
public:
    virtual ~IApplicationInterface() = default;

    virtual ICrashDetector *crashDetector() const = 0;
    virtual ILogger *logger() const = 0;
    virtual IFolderLocationService *folderLocationService() const = 0;
    virtual ISettingsService *settingsService() const = 0;
    virtual ILanguageService *languageService() const = 0;
    virtual ILastUsedFilesService *lastUsedFilesService() const = 0;
    virtual IIconService *iconService() const = 0;
};

}

#endif // SCHEDULEMASTER_H
