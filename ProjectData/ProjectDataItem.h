#ifndef PROJECTDATAITEM_H
#define PROJECTDATAITEM_H

#include "Global/Global.h"

#include <QObject>
#include <QUuid>
#include <QJsonObject>
#include <QJsonDocument>

#include "JavaScriptInterface/ProjectDataItemJsInterface.h"
#include "ProjectDataItemContainer.h"

//class ProjectDataItemJsInterface;

class ProjectDataItem : public QObject {
    Q_OBJECT
public:
    explicit ProjectDataItem(QObject *parent, const QUuid &id = QUuid(), const bool &isClone = false);

    QUuid id() const;
    QString idAsString() const;

    virtual QString comment() const = 0;
    virtual void setComment(const QString &comment) = 0;

    bool inUse() const;
    void setInUse(const bool &newInUse);

    virtual QList<ProjectDataItem *> activeChildren() const = 0;

    virtual bool isClone() const = 0;

    QString dump(const QJsonDocument::JsonFormat &format = QJsonDocument::Compact) const;
    void dumpToDebug(const QJsonDocument::JsonFormat &format = QJsonDocument::Compact) const;

    virtual ProjectDataItem *cloneBasePtr(QObject *parent = nullptr) const = 0;
    virtual ProjectDataItem *duplicateBasePtr(QObject *parent = nullptr) const = 0;
    virtual void mergeBasePtr(ProjectDataItem *other) = 0;

    virtual ProjectDataItem *originalBasePtr() const = 0;
    virtual ProjectDataItem *originBasePtr() const = 0;


    // TODO: Aus irgendeinem Grund wird nach dem Klonen über die JS Engine, also dem "verwenden" vom Interface der interface-Pointer selbst nullptr???
    template<typename T>
    ProjectDataItemJsInterface *createJsInterface() {
        if constexpr(!std::is_base_of<ProjectDataItemJsInterface, T>::value) {
            qCritical() << "Cannot create JS Interface, because it's not derived from "
                           "ProjectDataItemJsInterface class.";
            return nullptr;
        }

        if(isClone())
            _jsInterface = originBasePtr()->jsInterface();
        else if(_jsInterface) {
            qWarning() << "JS Interface already created.";
            return _jsInterface;
        } else {
            _jsInterface = new T(this);
            _jsInterface->setTargetBasePtr(this);
        }
        return _jsInterface;
    }

    ProjectDataItemJsInterface *jsInterface() const;
    void setJsInterface(ProjectDataItemJsInterface *interface);

    QList<ProjectDataItem *> jsModifiedItems() const;

    virtual QJsonObject toJson() const;

    template<typename T>
    T *findParent() const {
        QObject *obj = parent();
        while(obj) {
            if(T *casted = qobject_cast<T *>(obj))
                return casted;

            obj = obj->parent();
        }
        return nullptr;
    }

signals:
    void changed();

protected:
    virtual void fromJson(const QJsonObject &jsonObject) {
        const QUuid id = QUuid::fromString(jsonObject.value("id").toString());
        setID(id.isNull() ? QUuid::createUuid() : id);
        setComment(jsonObject.value("comment").toString());
    }

    void setID(const QUuid &newID) {
        _id = newID;
        QObject::setObjectName(idAsString());
    }

private:
    QUuid _id;
    bool _inUse;

    ProjectDataItemJsInterface *_jsInterface;
};

template <typename DerivedType, typename DataType>
class ProjectDataItemCRTP : public ProjectDataItem {
protected:
    explicit ProjectDataItemCRTP(QObject *parent, const QUuid &id = QUuid(), DerivedType *original = nullptr) :
        ProjectDataItem(parent, id, original),
        _original(original) {
    }

public:
    virtual QString comment() const override { return _data.comment(); }
    virtual void setComment(const QString &newComment) override {
        _data.setComment(newComment);
        emit changed();
    }

    virtual bool isClone() const override {
        return _original;
    }

    virtual QList<ProjectDataItem *> activeChildren() const override {
        QList<ProjectDataItem *> result;
        for(ProjectDataItemContainer *container :
            const_cast<ProjectDataItemCRTP<DerivedType, DataType> *>(this)
                ->_data.parentOwnsItemsMembersList())
            for(ProjectDataItem *item : container->valuesBasePtr())
                result.append(item);

        return result;
    }

    DataType data() const {
        return _data;
    }

    DataType cloneData(QObject *parent) const {
        return _data.clone(parent);
    }

    DataType duplicateData(QObject *parent) const {
        return _data.duplicate(parent);
    }

    void setData(const DataType &newData) {
        _data = newData;
        emit changed();
    }

    void mergeData(DataType mergeData) {
        _data.merge(mergeData, this);
        if(jsInterface())
            jsInterface()->setTargetBasePtr(this);
        emit changed();
    }

    DerivedType *clone(QObject *parent = nullptr) const {
        if(!parent)
            parent = this->parent();

        DerivedType *clone = new DerivedType(parent,
                                             id(),
                                             const_cast<DerivedType *>(
                                                 static_cast<const DerivedType *>(this)));
        clone->setObjectName(objectName() + "-clone");
        clone->setData(cloneData(clone));
        if(jsInterface())
            jsInterface()->setTargetBasePtr(clone);
        return clone;
    }

    virtual ProjectDataItem *cloneBasePtr(QObject *parent = nullptr) const override {
        return clone(parent);
    }

    DerivedType *duplicate(QObject *parent = nullptr) const {
        if(!parent)
            parent = this->parent();

        DerivedType *duplicate = new DerivedType(parent, QUuid::createUuid());
        duplicate->setData(duplicateData(duplicate));
        duplicate->setObjectName(idAsString());
        return duplicate;
    }

    virtual ProjectDataItem *duplicateBasePtr(QObject *parent = nullptr) const override {
        return duplicate(parent);
    }

    void merge(DerivedType *other) {
        _data.merge(other->data(), this);
        emit changed();
    }

    virtual void mergeBasePtr(ProjectDataItem *other) override {
        DerivedType *otherDerived = dynamic_cast<DerivedType *>(other);
        if(!otherDerived) {
            qDebug() << "Cannot use mergeBasePtr - Passed object pointer has not a matching type";
            return;
        }

        merge(otherDerived);
    }

    DerivedType *original() const {
        return _original;
    }

    virtual ProjectDataItem *originalBasePtr() const override {
        return original();
    }

    DerivedType *origin() const {
        DerivedType *current = const_cast<DerivedType *>(static_cast<const DerivedType *>(this));
        while (current->original())
            current = current->original();

        return current;
    }

    virtual ProjectDataItem *originBasePtr() const override {
        return origin();
    }

protected:
    DataType _data;

private:
    DerivedType *_original;
};

#endif // PROJECTDATAITEM_H
