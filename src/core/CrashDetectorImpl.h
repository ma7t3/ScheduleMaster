#ifndef CRASHDETECTORIMPL_H
#define CRASHDETECTORIMPL_H

#include <QObject>

#include "Service.h"
#include "src/api/ICrashDetector.h"

namespace ScheduleMaster::Core {

class CrashDetectorImpl : public QObject, public Service<CrashDetectorImpl>, public ICrashDetector {
    Q_OBJECT

public:
    explicit CrashDetectorImpl(QObject *parent = nullptr);
    void shutdown();

    virtual bool crashDetected() const override;

signals:

private:
    bool _crashDetected;
};
}

#endif // CRASHDETECTORIMPL_H
