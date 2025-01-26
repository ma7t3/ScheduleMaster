#ifndef BUSSTOPPLATFORM_H
#define BUSSTOPPLATFORM_H

#include "ProjectDataItem.h"
#include "ProjectDataItemData.h"

/**
 * @enum BusstopPlatformFlag
 * @brief The BusstopPlatformFlag enum describes flags that BusstopPlatforms can have. You can understand it as certain features a BusstopPlatform can have or not.
 */
enum class BusstopPlatformFlag : int {
    StandardBusstopPlatform = 0x0, ///< Standard platform without special features.
    ArrivalBusstopPlatform = 0x1, ///< Platform can be used by ending routes to arrive.
    BreakBusstopPlatform = 0x2, ///< Platform can be used by buses for breaks between trips.
    DepartureBusstopPlatform = 0x4, ///< Platform can be used by starting routes to departure.
    WaitBusstopPlatform = 0x8, ///< Platform can be used by buses to wait (e.g. if they are too early).
    InternalBusstopPlatform = 0x10 ///< Platform is used internally but not by passengers.
};
Q_DECLARE_FLAGS(BusstopPlatformFlags, BusstopPlatformFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(BusstopPlatformFlags)

/**
 * @struct BusstopPlatformData
 * @brief The BusstopPlatformData class contains the actual data of a BusstopPlatform object.
 *
 * It is seperated from the class logic to make it easier to change or completly replace it.
 */
struct BusstopPlatformData : ProjectDataItemData {
    BusstopPlatformData() {updateParentOwnsItemsMembers();}
    QList<ProjectDataItemContainer *> defineParentOwnsItemsMembers() override {return {};}

    /// The BusstopPlatform's name
    QString name;

    /// The BusstopPlatform's flags
    BusstopPlatformFlags flags;
};


/**
 * @class BusstopPlatform
 * @brief The BusstopPlatform class represents a platform that belongs to a busstop.
 *
 * One busstop has a name and certain flags that describe its features.
 */

class BusstopPlatform : public ProjectDataItem<BusstopPlatform, BusstopPlatformData> {
    Q_OBJECT
public:
    /**
     * @brief Constructs a new BusstopBusstopPlatform object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param id The optional ID to be used. It can't be changed later.
     */
    explicit BusstopPlatform(QObject *parent, const QUuid &id = QUuid());

    /**
     * @brief Constructs a new BusstopBusstopPlatform object by parsing a JSON object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param jsonObject The JSON object to parse.
     */
    explicit BusstopPlatform(QObject *parent, const QJsonObject &jsonObject);

    /**
     * @brief Compares this BusstopPlatform with other by their names.
     * @param other
     * @return Whether this BusstopPlatform's name is smaller than the other's name.
     */
    bool operator<(const BusstopPlatform &other) const;

    /**
     * @brief Returns the BusstopPlatform's name.
     *
     * See also setName().
     * @return The BusstopPlatform's name
     */
    QString name() const;

    /**
     * @brief Replaces the BusstopPlatform's name.
     * @param newName The new name
     *
     * See also name().
     */
    void setName(const QString &newName);

    /**
     * @brief Returns the BusstopPlatform's flags.
     *
     * See also setFlags().
     * @return The BusstopPlatform's flags
     */
    BusstopPlatformFlags flags() const;

    /**
     * @brief Replaces the BusstopPlatform's flags.
     * @param newFlags The new flags
     *
     * See also flags().
     */
    void setFlags(const BusstopPlatformFlags &newFlags);

    QJsonObject toJson() const override;

protected:
    void fromJson(const QJsonObject &jsonObject) override;
};

#endif // BUSSTOPPLATFORM_H
