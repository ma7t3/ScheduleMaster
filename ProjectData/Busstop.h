#ifndef BUSSTOP_H
#define BUSSTOP_H

#include "BusstopPlatform.h"
#include "ProjectDataItemSet.h"
#include "ProjectDataItemData.h"

#include <QObject>
#include <QJsonArray>

/**
 * @enum BusstopFlag
 * @brief The BusstopFlag enum describes flags that Busstops can have. You can understand it as certain features a Busstop can have or not.
 */
enum BusstopFlag : int {
    StandardBusstop = 0x0, ///< Standard busstop without special features.
    FinalBusstop = 0x1, ///< Busstop is final busstop (Routes can end here).
    TransferBusstop = 0x2, ///< Busstop is transfer busstop (Passengers are recommended to change here).
    CentralTransferBusstop = 0x4, ///< Busstop is central transfer busstop (Multiple routes come together here; Passengers are recommended to change here).
    InternalBusstop = 0x8 ///< Busstop is used internally but not by passengers.
};

Q_DECLARE_FLAGS(BusstopFlags, BusstopFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(BusstopFlags)

/**
 * @struct BusstopData
 * @brief The BusstopData class contains the actual data of a Busstop object.
 *
 * It is seperated from the class logic to make it easier to change or completly replace it.
 */
struct BusstopData : ProjectDataItemData<BusstopData> {
    /// Constructs a new BusstopData object. It should always call ProjectDataItemData::initParentOwnsItemMembers().
    BusstopData() {initParentOwnsItemMembers();}

    QList<ProjectDataItemContainer *> parentOwnsItemsMembersList() override {return {&platforms};}

    /// The Busstop's name
    QString name;

    /// The Busstop's flags
    BusstopFlags flags;

    /// The Busstop's platforms
    PDISet<BusstopPlatform> platforms;

    /**
     * @brief The Id of the Busstop's default platform.
     *
     * This can be a null id if the Busstop has no default BusstopPlatform set.
     *
     * **Important Note:** Always use the Busstop::defaultPlatform() method to get the default platform.
     * This value may conaints an id that doesn't belong to any platform in the platform list.
     */
    QUuid defaultPlatform;
};


/**
 * @class Busstop
 * @brief The Busstop class represents one busstop.
 *
 * One busstop has a name and certain flags that describe its features.
 * It can have multiple platforms where buses can stop.
 */

class Busstop : public ProjectDataItem<Busstop, BusstopData> {
    Q_OBJECT
public:
    /**
     * @brief Constructs a new Busstop object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param id The optional ID to be used. It can't be changed later.
     * @param isClone Specifies if the object is created as a clone.
     */
    explicit Busstop(QObject *parent, const QUuid &id = QUuid(), const bool &isClone = false);

    /**
     * @brief Constructs a new Busstop object by parsing a JSON object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param jsonObject The JSON object to parse.
     */
    explicit Busstop(QObject *parent, const QJsonObject &jsonObject);

    /**
     * @brief Compares this Busstop with other by their names.
     * @param other
     * @return Whether this Busstop's name is smaller than the other's name.
     */
    bool operator<(const Busstop &other) const;

    /**
     * @brief Returns the Busstop's name.
     *
     * See also setName().
     * @return The Busstop's name
     */
    QString name() const;

    /**
     * @brief Replaces the Busstop's name.
     * @param newName The new name
     *
     * See also name().
     */
    void setName(const QString &newName);

    /**
     * @brief Returns the Busstop's flags.
     *
     * See also setFlags().
     * @return The Busstop's flags
     */
    BusstopFlags flags() const;

    /**
     * @brief Replaces the Busstop's flags.
     * @param newFlags The new flags
     *
     * See also flags().
     */
    void setFlags(const BusstopFlags &newFlags);

    /**
     * @brief Returns the Busstop's platform count
     *
     * See also platforms().
     * @return The number of platforms in the busstop
     */
    int platformCount() const;

    /**
     * @brief Searches for a platform by its UUID.
     * @param id The UUID of the platform to search for.
     * @return A pointer to the platform if it was found, otherwise nullptr.
     */
    BusstopPlatform *platform(const QUuid &id) const;

    /**
     * @brief Returns a list of all platforms in the busstop.
     *
     * See also platformCount().
     * @return A ProjectDataItemSet of all platforms in the busstop.
     */
    PDISet<BusstopPlatform> platforms() const;

    /**
     * @brief Adds a platform to the busstop.
     * @param platform The BusstopPlatform to add.
     */
    void addPlatform(BusstopPlatform *platform);

    /**
     * @brief Removes a platform from the busstop. This does nothing if the given platform is not part of the busstop or is nullptr.
     * @param platform The BusstopPlatform to remove.
     */
    void removePlatform(BusstopPlatform *platform);

    /**
     * @brief Removes a platform from the busstop by its UUID. This does nothing if there was no platform found that matches the given UUID.
     * @param id The id to search for.
     */
    void removePlatform(const QUuid &id);

    /**
     * @brief Finds all platforms that have a certain flag set.
     * @param flag The flag to filter by
     * @return A ProjectDataItemSet of all platforms that have the flag set.
     */
    PDISet<BusstopPlatform> platformsWithFlag(const BusstopPlatformFlag &flag) const;

    /**
     * @brief Returns the default platform of the busstop.
     *
     * If the busstop has no default platform, this returns nullptr.
     * @return A pointer to the Busstop's default platform
     */
    BusstopPlatform *defaultPlatform() const;

    /**
     * @brief Check if a given platform is the Busstop's default platform.
     *
     * This also returns false if the given platform does not belong to the busstop or is nulllptr.
     * @param platform The BusstopPlatform to check
     * @return true if platform is the Busstop's default BusstopPlatform
     */
    bool isDefaultPlatform(BusstopPlatform *platform) const;

    /**
     * @brief Sets a new default platform.
     * @param platform The BusstopPlatform to set as the new default platform
     */
    void setDefaultPlatform(BusstopPlatform *platform);

    QJsonObject toJson() const override;

protected:
    void fromJson(const QJsonObject &jsonObject) override;
};

#endif // BUSSTOP_H

