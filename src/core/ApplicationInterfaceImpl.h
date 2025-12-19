#ifndef SCHEDULEMASTERIMPL_H
#define SCHEDULEMASTERIMPL_H

#include <QObject>
#include "src/api/ScheduleMaster.h"
#include "src/core/CrashDetectorImpl.h"

namespace ScheduleMaster::Core {

#define app ApplicationInterfaceImpl::instance()

class ApplicationInterfaceImpl : public QObject, public IApplicationInterface {
    Q_OBJECT

public:
    explicit ApplicationInterfaceImpl(QObject *parent);
    static ApplicationInterfaceImpl *instance();

    virtual ICrashDetector *crashDetector() const override;

protected:
    static inline ApplicationInterfaceImpl *_self = nullptr;

private:
    CrashDetectorImpl *_crashDetector;
};

}

#endif // SCHEDULEMASTERIMPL_H
