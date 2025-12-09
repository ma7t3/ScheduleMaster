#ifndef BUSSTOPJSINTERFACE_H
#define BUSSTOPJSINTERFACE_H

#include "ProjectDataItemJsInterface.h"

#include "ProjectData/Busstop.h"

class BusstopJsInterface : public ProjectDataItemJsInterfaceCRTP<Busstop> {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(BusstopFlagsJsInterface flags READ flags WRITE setFlags NOTIFY flagsChanged)

public:
    explicit BusstopJsInterface(QObject *parent = nullptr);

    enum BusstopFlagJsInterface : int {
        StandardBusstop = 0x0,
        FinalBusstop = 0x1,
        TransferBusstop = 0x2,
        CentralTransferBusstop = 0x4,
        InternalBusstop = 0x8
    };

    Q_ENUM(BusstopFlagJsInterface);
    Q_DECLARE_FLAGS(BusstopFlagsJsInterface, BusstopFlagJsInterface)

    QString name() const;
    void setName(const QString &name);

    BusstopFlagsJsInterface flags() const;
    void setFlags(const BusstopFlagsJsInterface &flags);

protected:
    virtual void connectToTarget(Busstop *target) override;
    virtual void disconnectFromTarget(Busstop *target) override;

signals:
    void nameChanged();
    void flagsChanged();
};


#endif // BUSSTOPJSINTERFACE_H
