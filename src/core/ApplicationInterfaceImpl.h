#ifndef SCHEDULEMASTERIMPL_H
#define SCHEDULEMASTERIMPL_H

#include <QObject>
#include "src/api/ScheduleMaster.h"

namespace ScheduleMaster::Core {

#define app ApplicationInterfaceImpl::instance()

class ApplicationInterfaceImpl : public QObject, public IApplicationInterface {
    Q_OBJECT

public:
    ApplicationInterfaceImpl(QObject *parent);
    static ApplicationInterfaceImpl *instance();

protected:
    static inline ApplicationInterfaceImpl *_self = nullptr;
};

}

#endif // SCHEDULEMASTERIMPL_H
