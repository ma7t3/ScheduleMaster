#ifndef PROJECTDATAITEMJSINTERFACE_H
#define PROJECTDATAITEMJSINTERFACE_H

#include <QObject>
#include <QDebug>
#include <QUndoStack>

class ProjectDataItem;

class ProjectDataItemJsInterface : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)

public:
    explicit ProjectDataItemJsInterface(QObject *parent) : QObject(parent){}

    virtual QString id() const = 0;

    virtual ProjectDataItem *targetBasePtr() = 0;
    virtual void setTargetBasePtr(ProjectDataItem *target) = 0;

    virtual bool isModified() const = 0;

    virtual void cancelOperation() = 0;
    virtual void endOperation() = 0;

protected:
    virtual void startOperation() = 0;

signals:
    void targetChanged();
};

template<typename TargetType>
class ProjectDataItemJsInterfaceCRTP : public ProjectDataItemJsInterface {

public:
    explicit ProjectDataItemJsInterfaceCRTP(QObject *parent = nullptr) : ProjectDataItemJsInterface(parent), _target(nullptr), _operationOriginal(nullptr) {}

    TargetType *target() const {
        return _target;
    }

    virtual ProjectDataItem *targetBasePtr() override {
        return target();
    }

    void setTarget(TargetType *target) {
        if(_target)
            disconnectFromTarget(_target);
        _target = target;
        if(_target)
            connectToTarget(_target);
        emit targetChanged();
    }

    virtual void setTargetBasePtr(ProjectDataItem *target) override {
        TargetType *upcastTarget = dynamic_cast<TargetType *>(target);
        if(!upcastTarget) {
            qCritical() << "Cannot use setTargetBasePtr! Cast failed!";
            return;
        }

        setTarget(upcastTarget);
    }

    virtual QString id() const override { return _target->idAsString(); }

    virtual bool isModified() const override {
        return _operationOriginal;
    }

    virtual void cancelOperation() override {
        if(!isModified())
            qDebug() << "No operation started";
            return;

        _target->deleteLater();
        _target = _operationOriginal;
        _operationOriginal = nullptr;
    }

    virtual void endOperation() override {
        if(!isModified())
            return;

        _target = _operationOriginal;
        _operationOriginal = nullptr;
    }

protected:
    virtual void startOperation() override {
        if(isModified())
            return;

        qDebug() << _target->jsInterface();

        _operationOriginal = _target;
        setTarget(_target->clone());
    }

    virtual void connectToTarget(TargetType *target) {};
    virtual void disconnectFromTarget(TargetType *target) {};

    TargetType *_target, *_operationOriginal;
};

#endif // PROJECTDATAITEMJSINTERFACE_H
