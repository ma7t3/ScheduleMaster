#ifndef PROJECTDATAITEM_H
#define PROJECTDATAITEM_H

#include "ProjectDataItemSignals.h"
#include "Global/Global.h"

#include <QObject>
#include <QUuid>
#include <QJsonObject>
#include <QJsonDocument>

/**
 * @class ProjectDataItem
 * @brief The ProjectDataItem class is an abstract parent class for all data items in the project (except the ProjectData itself).
 *
 * It especially provides a unique ID for each item.
 *
 * In addition, it holds information about whether the item is currently in use or not and if it is a clone.
 *
 * **Note:** This class is a template class. It takes the derived class and the data class as template parameters.
 * For example, the Busstop class subclasses ProjectDataItem<Busstop, BusstopData>.
 *
 * Since Qt-Signals and Slots can't be defined inside template classes, this class inherits from ProjectDataItemSignals which just provides the signals.
 */

template <typename DerivedType, typename DataType>
class ProjectDataItem : public ProjectDataItemSignals {
protected:
    /**
     * @brief Constructs a new ProjectDataItem object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param id The optional ID to be used. It can't be changed later.
     */
    explicit ProjectDataItem(QObject *parent, const QUuid &id = QUuid(), DerivedType *original = nullptr) :
        ProjectDataItemSignals(parent),
        _inUse(false),
        _original(original) {
        setID(id.isNull() ? generateID() : id);
    }

public:
    /**
     * @brief Returns the UUID of the ProjectDataItem.
     *
     * See also idAsString() and generateID().
     * @return The UUID
     */
    QUuid id() const { return _id; }

    /**
     * @brief Returns the UUID of the ProjectDataItem as a string.
     *
     * See also id() and generateID().
     * @return The UUID as a string
     */
    QString idAsString() const { return id().toString(QUuid::WithoutBraces); }

    /**
     * @brief Returns the internal comment of the ProjectDataItem.
     * @return The internal comment
     */
    QString comment() const { return _data.comment(); }

    /**
     * @brief Sets the internal comment of the ProjectDataItem to newComment.
     * @param newComment The comment to set
     */
    void setComment(const QString &newComment) {
        _data.setComment(newComment);
        emit changed();
    }

    /**
     * @brief Generates and returns a new UUID.
     *
     * See also id() and idAsString().
     * @return The generated UUID
     */
    static QUuid generateID() { return QUuid::createUuid(); }

    /**
     * @brief Returns if the item is currently in use.
     *
     * It will always return true if the item is a clone.
     *
     * See also isClone().
     * @return Whether the item is in use or not.
     */
    bool inUse() const { return isClone() ? true : _inUse; }

    /**
     * @brief Change the isUsed status of the item.
     * @param newInUse The new isUsed status
     */
    void setInUse(const bool &newInUse) { _inUse = newInUse; }

    /**
     * @brief Converts the ProjectDataItem to a JSON object
     *
     * This only converts the general ProjectDatItem-specific data. Derived classes should override this method to add their own data.
     * But they should call ProjectDataItem::toJson() first and use the returned object to add their own data.
     *
     * See also fromJson().
     * @return the QJsonObject
     */
    virtual QJsonObject toJson() const {
        return QJsonObject {
            {"object", metaObject()->className()},
            {"id", idAsString()},
            {"comment", comment()}
        };
    }

    /**
     * @brief Dumps the ProjectDataItem to a JSON string including additional information like the memory address, the Type (derived class name), inUse property and isClone property.
     * @param format The JSON format to use
     * @return
     */
    QString dump(const QJsonDocument::JsonFormat &format = QJsonDocument::Compact) const {
        QJsonObject jsonObject = toJson();
        jsonObject.insert("_address", QString("0x%1").arg(reinterpret_cast<quintptr>(this), 0, 16).toUpper());
        jsonObject.insert("_type", metaObject()->className());
        jsonObject.insert("_inUse", _inUse);
        jsonObject.insert("_isClone", isClone());
        return QJsonDocument(jsonObject).toJson(format);
    }

    /**
     * @brief Runs dump() and writes the data to the qDebug() output.
     * @param format The JSON format to use
     */
    void dumpToDebug(const QJsonDocument::JsonFormat &format = QJsonDocument::Compact) const {
        qDebug().noquote() << dump(format);
    }

    /**
     * @brief Returns the ProjectDataItem's data.
     *
     * See also cloneData() and setData().
     *
     * **Attention:** This does not clone the data.
     * If you want to modify the data without changing "originals", you should use cloneData() instead.
     * @return The ProjectDataItem's data.
     */
    DataType data() const {
        return _data;
    }

    /**
     * @brief Returns a cloned version of the ProjectDataItem's data.
     *
     * Cloned means, that it will create a deep copy of the data (Recursively cloning all sub-items in containers).
     * That means that the data can be modified without changing the original data.
     *
     * See also data() and setData().
     * @return
     */
    DataType cloneData(QObject *parent) const {
        return _data.clone(parent);
    }

    DataType duplicateData(QObject *parent) const {
        return _data.duplicate(parent);
    }

    /**
     * @brief Replaces the ProjectDataItem's data.
     * @param newData The new data
     *
     * See also data() and cloneData().
     */
    void setData(const DataType &newData) {
        _data = newData;
        emit changed();
    }

    /**
     * @brief Merges a new data object into the current one.
     * @param mergeData The new data to merge
     */
    void mergeData(DataType mergeData) {
        _data.merge(mergeData, this);
        emit changed();
    }

    /**
     * @brief Returns if the item is a clone.
     * @return Whether the item is a clone or not.
     */
    bool isClone() const {
        return _original;
    }

    DerivedType *original() const {
        return _original;
    }

    DerivedType *origin() const {
        DerivedType *current = const_cast<DerivedType *>(static_cast<const DerivedType *>(this));
        while (current->original())
            current = current->original();

        return current;
    }

    /**
     * @brief Returns a cloned version of the item.
     *
     * Cloned means, that it will also create a deep copy of the data (Recursively cloning all sub-items in containers).
     * That means that the data can be modified without changing the original data.
     *
     * **Note:** This changes the objectName to the id formated as a string with "-clone" appended.
     * @return
     */
    DerivedType *clone(QObject *parent = nullptr) const {
        if(!parent)
            parent = this->parent();

        DerivedType *clone = new DerivedType(parent,
                                             id(),
                                             const_cast<DerivedType *>(
                                                 static_cast<const DerivedType *>(this)));
        clone->setObjectName(objectName() + "-clone");
        clone->setData(cloneData(clone));
        return clone;
    }

    DerivedType *duplicate(QObject *parent = nullptr) const {
        if(!parent)
            parent = this->parent();

        DerivedType *duplicate = new DerivedType(parent, generateID());
        duplicate->setData(duplicateData(duplicate));
        duplicate->setObjectName(idAsString());
        return duplicate;
    }

protected:
    /**
     * @brief Reads a JSON object and fills the ProjectDataItem with the data from the JSON object.
     *
     * This only reads the general ProjectDatItem-specific data. Derived classes should override this method to read their own data.
     * But they should call ProjectDataItem::fromJson() first.
     *
     * See also toJson().
     * @param jsonObject The QJsonObject to read from.
     */
    virtual void fromJson(const QJsonObject &jsonObject) {
        QUuid id = QUuid::fromString(jsonObject.value("id").toString());
        setID(id.isNull() ? generateID() : id);
        setComment(jsonObject.value("comment").toString());
    }

    /**
     * @brief Sets a new id.
     *
     * **Attention:** Be careful with using this. You should only set the id if the item is created. But the it never should change later!
     *
     * This also sets the QObject::objectName to the id formated as a string.
     * @param newID The new id
     */
    void setID(const QUuid &newID) {
        _id = newID;
        QObject::setObjectName(idAsString());
    }

    /// The ProjectDataItem's data
    DataType _data;

private:
    /**
     * @brief The UUID of the ProjectDataItem.
     */
    QUuid _id;

    /**
     * @brief The inUse property.
     *
     * This property specified whether the item is used by other items at the moment.
     * This only includes other items are parents of this item but not other items that just reference it.
     *
     * This means:
     * - If a route is part of a line, inUse should be true.
     * - If the route is removed from the line (but stays in the memeory because of undo support), inUse should be false.
     * - If the route is used by a trip or not should not be releant for inUse.
     */
    bool _inUse;

    DerivedType *_original;
};

#endif // PROJECTDATAITEM_H
