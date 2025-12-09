#ifndef BUSSTOPPLATFORMJSINTERFACE_H
#define BUSSTOPPLATFORMJSINTERFACE_H

#include "ProjectDataItemJsInterface.h"
#include "ProjectData/BusstopPlatform.h"

class BusstopPlatformJsInterface : public ProjectDataItemJsInterfaceCRTP<BusstopPlatform> {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(BusstopPlatformFlagsJsInterface flags READ flags WRITE setFlags NOTIFY flagsChanged)

public:
    explicit BusstopPlatformJsInterface(QObject *parent = nullptr);

    enum BusstopPlatformFlagJsInterface : int {
        StandardBusstopPlatform = 0x0,
        ArrivalBusstopPlatform = 0x1,
        BreakBusstopPlatform = 0x2,
        DepartureBusstopPlatform = 0x4,
        WaitBusstopPlatform = 0x8,
        InternalBusstopPlatform = 0x10
    };

    Q_ENUM(BusstopPlatformFlagJsInterface)
    Q_DECLARE_FLAGS(BusstopPlatformFlagsJsInterface, BusstopPlatformFlagJsInterface)

    QString name() const;
    void setName(const QString &name);

    BusstopPlatformFlagsJsInterface flags() const;
    void setFlags(const BusstopPlatformFlagsJsInterface &flags);

protected:
    virtual void connectToTarget(BusstopPlatform *target) override;
    virtual void disconnectFromTarget(BusstopPlatform *target) override;

signals:
    void nameChanged();
    void flagsChanged();
};

#endif // BUSSTOPPLATFORMJSINTERFACE_H
