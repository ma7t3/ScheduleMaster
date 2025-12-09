#include "BusstopJsInterface.h"

BusstopJsInterface::BusstopJsInterface(QObject *parent) : ProjectDataItemJsInterfaceCRTP<Busstop>(parent) {}

QString BusstopJsInterface::name() const {
    return target()->name();
}

void BusstopJsInterface::setName(const QString &name) {
    startOperation();
    target()->setName(name);
}

BusstopJsInterface::BusstopFlagsJsInterface BusstopJsInterface::flags() const {
    return BusstopFlagsJsInterface(_target->flags().toInt());
}

void BusstopJsInterface::setFlags(const BusstopFlagsJsInterface &flags) {
    startOperation();
    _target->setFlags(BusstopFlags(flags.toInt()));
}

void BusstopJsInterface::connectToTarget(Busstop *target) {
    connect(target, &Busstop::changed, this, &BusstopJsInterface::nameChanged);
    connect(target, &Busstop::changed, this, &BusstopJsInterface::flagsChanged);
}

void BusstopJsInterface::disconnectFromTarget(Busstop *target) {
    target->disconnect(this);
}
