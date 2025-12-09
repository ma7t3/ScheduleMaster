#include "BusstopJsInterface.h"

#include "JavaScriptInterface.h"

BusstopJsInterface::BusstopJsInterface(QObject *parent) : ProjectDataItemJsInterfaceCRTP<Busstop>(parent) {}

QString BusstopJsInterface::name() const {
    return target()->name();
}

void BusstopJsInterface::setName(const QString &name) {
    startOperation();
    target()->setName(name);
}

BusstopJsInterface::BusstopFlagsJsInterface BusstopJsInterface::flags() const {
    return BusstopFlagsJsInterface(target()->flags().toInt());
}

void BusstopJsInterface::setFlags(const BusstopFlagsJsInterface &flags) {
    startOperation();
    target()->setFlags(BusstopFlags(flags.toInt()));
}

QJSValue BusstopJsInterface::platforms() const {
    return JavaScriptInterface::projectDataItemsToJsArray(target()->platforms(), this);
}

void BusstopJsInterface::connectToTarget(Busstop *target) {
    connect(target, &Busstop::changed, this, &BusstopJsInterface::nameChanged);
    connect(target, &Busstop::changed, this, &BusstopJsInterface::flagsChanged);
    connect(target, &Busstop::platformAdded, this, &BusstopJsInterface::platformsChanged);
    connect(target, &Busstop::platformRemoved, this, &BusstopJsInterface::platformsChanged);
}

void BusstopJsInterface::disconnectFromTarget(Busstop *target) {
    target->disconnect(this);
}
