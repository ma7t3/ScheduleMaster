#include "ProjectDataItem.h"

#include "ProjectDataItemContainer.h"

ProjectDataItem::ProjectDataItem(QObject *parent, const QUuid &id, const bool &isClone) : QObject(parent), _inUse(false), _jsInterface(nullptr) {
    setID(id.isNull() ? QUuid::createUuid() : id);
}

QUuid ProjectDataItem::id() const {
    return _id;
}

QString ProjectDataItem::idAsString() const {
    return id().toString(QUuid::WithoutBraces);
}

bool ProjectDataItem::inUse() const {
    return isClone() ? true : _inUse;
}

void ProjectDataItem::setInUse(const bool &newInUse) {
    _inUse = newInUse;
}

QString ProjectDataItem::dump(const QJsonDocument::JsonFormat &format) const {
    QJsonObject jsonObject = toJson();
    jsonObject.insert("_address",
                      QString("0x%1").arg(reinterpret_cast<quintptr>(this), 0, 16).toUpper());
    jsonObject.insert("_type", metaObject()->className());
    jsonObject.insert("_inUse", inUse());
    jsonObject.insert("_isClone", isClone());
    return QJsonDocument(jsonObject).toJson(format);
}

void ProjectDataItem::dumpToDebug(const QJsonDocument::JsonFormat &format) const {
    qDebug().noquote() << dump(format);
}

ProjectDataItemJsInterface *ProjectDataItem::jsInterface() const {
    return isClone() ? originBasePtr()->jsInterface() : _jsInterface;
}

QList<ProjectDataItem *> ProjectDataItem::jsModifiedItems() const {
    if(!jsInterface())
        return {};

    if(jsInterface()->isModified())
        return {jsInterface()->targetBasePtr()};

    QList<ProjectDataItem *> result;
    for(ProjectDataItem *item : activeChildren())
        result.append(item->jsModifiedItems());

    return result;
}

QJsonObject ProjectDataItem::toJson() const {
    return QJsonObject{{"object", metaObject()->className()},
                       {"id", idAsString()},
                       {"comment", comment()}};
}
