#ifndef SCHEDULEMASTERIMPL_H
#define SCHEDULEMASTERIMPL_H

#include <QObject>
#include "src/api/ScheduleMaster.h"

#define app ApplicationInterfaceImpl::instance()

namespace ScheduleMaster {
class ICrashDetector;
class ILogger;
class IFolderLocationService;
}

namespace ScheduleMaster::Core {

class CrashDetectorImpl;
class LoggerImpl;
class FolderLocationServiceImpl;

class ApplicationInterfaceImpl : public QObject, public IApplicationInterface {
    Q_OBJECT

public:
    explicit ApplicationInterfaceImpl(QObject *parent);
    static ApplicationInterfaceImpl *instance();

    virtual ICrashDetector *crashDetector() const override;
    virtual ILogger *logger() const override;
    virtual IFolderLocationService *folderLocationService() const override;

protected:
    static inline ApplicationInterfaceImpl *_self = nullptr;

private:
    CrashDetectorImpl *_crashDetector;
    LoggerImpl *_logger;
    FolderLocationServiceImpl *_folderLocationService;
};

}

#endif // SCHEDULEMASTERIMPL_H
