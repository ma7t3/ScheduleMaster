#ifndef SCHEDULEMASTERIMPL_H
#define SCHEDULEMASTERIMPL_H

#include <QObject>
#include "src/api/ScheduleMaster.h"

#define app ApplicationInterfaceImpl::instance()

namespace ScheduleMaster {
class ICrashDetector;
class ILogger;
}

namespace ScheduleMaster::Core {

class CrashDetectorImpl;
class LoggerImpl;

class ApplicationInterfaceImpl : public QObject, public IApplicationInterface {
    Q_OBJECT

public:
    explicit ApplicationInterfaceImpl(QObject *parent);
    static ApplicationInterfaceImpl *instance();

    virtual ICrashDetector *crashDetector() const override;
    virtual ILogger *logger() const override;

protected:
    static inline ApplicationInterfaceImpl *_self = nullptr;

private:
    CrashDetectorImpl *_crashDetector;
    LoggerImpl *_logger;
};

}

#endif // SCHEDULEMASTERIMPL_H
