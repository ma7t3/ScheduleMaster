#include "ProjectDataJsInterface.h"

#include "ProjectData/ProjectData.h"

#include <QJSEngine>

#include <JavaScriptInterface/JavaScriptInterface.h>

ProjectDataJsInterface::ProjectDataJsInterface(QObject *parent) : QObject(parent) {}

void ProjectDataJsInterface::setTarget(ProjectData *target) {
    _target = target;
}

QString ProjectDataJsInterface::filePath() const {
    return _target->filePath();
}

QJSValue ProjectDataJsInterface::busstops() const {
    return JavaScriptInterface::projectDataItemsToJsArray(_target->busstops(), this);
}

QJSValue ProjectDataJsInterface::lines() const {
    return JavaScriptInterface::projectDataItemsToJsArray(_target->lines(), this);
}

QJSValue ProjectDataJsInterface::busstop(const QString &id) const {
    return JavaScriptInterface::projectDataItemToJsObject(_target->busstop(QUuid::fromString(id)), this);
}

QJSValue ProjectDataJsInterface::line(const QString &id) const {
    return JavaScriptInterface::projectDataItemToJsObject(_target->line(QUuid::fromString(id)), this);
}
