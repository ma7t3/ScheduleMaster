#ifndef PROJECTDATAITEM_H
#define PROJECTDATAITEM_H

#include <QObject>
#include <QJsonObject>

/**
 * @class ProjectDataItem
 * @brief The ProjectDataItem class is an abstract parent class for all data items in the project (except the ProjectData itself).
 *
 * It especially provides a unique ID for each item.
 */

class ProjectDataItem : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs a new ProjectDataItem object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param id The optional ID to be used. It can't be changed later.
     */
    explicit ProjectDataItem(QObject *parent, const QUuid &id = QUuid());

    /**
     * @brief Returns the UUID of the ProjectDataItem.
     *
     * See also idAsString() and generateID().
     * @return The UUID
     */
    QUuid id() const;

    /**
     * @brief Returns the UUID of the ProjectDataItem as a string.
     *
     * See also id() and generateID().
     * @return The UUID as a string
     */
    QString idAsString() const;

    /**
     * @brief Generates and returns a new UUID.
     *
     * See also id() and idAsString().
     * @return The generated UUID
     */
    static QUuid generateID();

    /**
     * @brief Returns if the item is currently in use.
     * @return Whether the item is in use or not.
     */
    bool inUse() const;

    /**
     * @brief Change the isUsed status of the item.
     * @param newInUse The new isUsed status
     */
    void setInUse(const bool &newInUse);

    /**
     * @brief Converts the ProjectDataItem to a JSON object
     *
     * This only converts the general ProjectDatItem-specific data. Derived classes should override this method to add their own data.
     * But they should call ProjectDataItem::toJson() first and use the returned object to add their own data.
     *
     * See also fromJson().
     * @return the QJsonObject
     */
    virtual QJsonObject toJson() const;

signals:
    /**
     * @brief This signal is emited whenever the data is changed.
     *
     * This can be just one data property to be changed or the entire data object being replaced.
     */
    void changed();

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
    virtual void fromJson(const QJsonObject &jsonObject);

    /**
     * @brief Sets a new id.
     *
     * **Attention:** Be careful with using this. You should only set the id if the item is created. But the it never should change later!
     * @param newID The new id
     */
    void setID(const QUuid &newID);

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
};

#endif // PROJECTDATAITEM_H
