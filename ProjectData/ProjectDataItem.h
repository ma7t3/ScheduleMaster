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
     * @brief Converts the ProjectDataItem to a JSON object
     *
     * This only converts the general ProjectDatItem-specific data. Derived classes should override this method to add their own data.
     * But they should call ProjectDataItem::toJson() first and use the returned object to add their own data.
     *
     * See also fromJson().
     * @return the QJsonObject
     */
    QJsonObject toJson() const;

signals:

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
    void fromJson(const QJsonObject &jsonObject);

private:

    /**
     * @brief The UUID of the ProjectDataItem.
     */
    QUuid _id;
};

#endif // PROJECTDATAITEM_H
