#include "BusstopPlatformJsInterface.h"

BusstopPlatformJsInterface::BusstopPlatformJsInterface(QObject *parent) : ProjectDataItemJsInterfaceCRTP<BusstopPlatform>(parent) {}

QString BusstopPlatformJsInterface::name() const {
    return target()->name();
}

void BusstopPlatformJsInterface::setName(const QString &name) {
    startOperation();
    target()->setName(name);
}

BusstopPlatformJsInterface::BusstopPlatformFlagsJsInterface BusstopPlatformJsInterface::flags()
    const {
    return BusstopPlatformFlagsJsInterface(target()->flags().toInt());
}

void BusstopPlatformJsInterface::setFlags(const BusstopPlatformFlagsJsInterface &flags) {
    startOperation();
    target()->setFlags(BusstopPlatformFlags(flags.toInt()));
}

void BusstopPlatformJsInterface::connectToTarget(BusstopPlatform *target) {
    connect(target, &BusstopPlatform::changed, this, &BusstopPlatformJsInterface::nameChanged);
    connect(target, &BusstopPlatform::changed, this, &BusstopPlatformJsInterface::flagsChanged);
}

void BusstopPlatformJsInterface::disconnectFromTarget(BusstopPlatform *target) {
    target->disconnect(this);
}
