#ifndef SCHEDULEMASTERIMPL_H
#define SCHEDULEMASTERIMPL_H

#include <QObject>
#include <QDebug>
#include "src/api/ScheduleMaster.h"
#include "src/core/CrashDetectorImpl.h"

#define app ApplicationInterfaceImpl::instance()

namespace ScheduleMaster {
class ICrashDetector;
class ILogger;
class IFolderLocationService;
class ISettingsService;
class ILanguageService;
class ILastUsedFilesService;
class IIconService;
class IAppearanceService;
}

namespace ScheduleMaster::Core {

class CrashDetectorImpl;
class LoggerImpl;
class FolderLocationServiceImpl;
class SettingsServiceImpl;
class LanguageServiceImpl;
class LastUsedFilesServiceImpl;
class IconServiceImpl;
class AppearanceServiceImpl;

class ApplicationInterfaceImpl : public QObject, public IApplicationInterface {
    Q_OBJECT

public:
    explicit ApplicationInterfaceImpl(QObject *parent = nullptr);
    virtual ~ApplicationInterfaceImpl() { CrashDetectorImpl::instance()->shutdown(); }

    static ApplicationInterfaceImpl *instance();

    virtual ICrashDetector *crashDetector() const override;
    virtual ILogger *logger() const override;
    virtual IFolderLocationService *folderLocationService() const override;
    virtual ISettingsService *settingsService() const override;
    virtual ILanguageService *languageService() const override;
    virtual ILastUsedFilesService *lastUsedFilesService() const override;
    virtual IIconService *iconService() const override;
    virtual IAppearanceService *appearanceService() const override;

protected:
    static inline ApplicationInterfaceImpl *_self = nullptr;

private:
    CrashDetectorImpl *_crashDetector;
    LoggerImpl *_logger;
    FolderLocationServiceImpl *_folderLocationService;
    SettingsServiceImpl *_settingsService;
    LanguageServiceImpl *_languageService;
    LastUsedFilesServiceImpl *_lastUsedFilesService;
    IconServiceImpl *_iconService;
    AppearanceServiceImpl *_appearanceService;
};

}

#endif // SCHEDULEMASTERIMPL_H
