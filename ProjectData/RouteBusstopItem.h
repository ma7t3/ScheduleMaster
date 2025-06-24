#ifndef ROUTEBUSSTOPITEM_H
#define ROUTEBUSSTOPITEM_H

#include "Busstop.h"

/**
 * @struct RouteBusstopItemData
 * @brief The RouteBusstopItemData class contains the actual data of a RouteBusstopItem object.
 *
 * It is seperated from the class logic to make it easier to change or completly replace it.
 */
struct RouteBusstopItemData : ProjectDataItemData<RouteBusstopItemData> {
    /// Constructs a new RouteBusstopItemData object. It should always call ProjectDataItemData::initParentOwnsItemMembers().
    RouteBusstopItemData() {initParentOwnsItemMembers();}

    QList<ProjectDataItemContainer *> parentOwnsItemsMembersList() override {return {};}

    /// The RouteBusstopItem's Busstop
    Busstop *busstop = nullptr;

    /// The busstop's defaultPlatform in this RouteBusstopItem
    BusstopPlatform *defaultPlatform = nullptr;
};

/**
 * @brief The RouteBusstopItem class represents one busstop entry inside a Route.
 *
 * It contains information about which busstop is used and which platform is prefered.
 * In addition, one Route can use the same Busstop multiple times. So there will be multiple differen RouteBusstopItems referencing the same Busstop.
 */
class RouteBusstopItem : public ProjectDataItem<RouteBusstopItem, RouteBusstopItemData> {
    friend class Route;
    friend class ProjectDataItem<RouteBusstopItem, RouteBusstopItemData>;
    Q_OBJECT
private:
    /**
     * @brief Constructs a new RouteBusstopItem object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param busstop The busstop to be used in this RouteBusstopItem
     * @param id The optional ID to be used. It can't be changed later.
     * @param isClone Specifies if the object is created as a clone.
     */
    explicit RouteBusstopItem(QObject *parent, Busstop *busstop, const QUuid &id = QUuid(), const bool &isClone = false);

    /**
     * @brief Constructs a new RouteBusstopItem object.
     *
     * If no ID was specified a new ID is generated.
     *
     * **Note:** When using the constructor, the busstop is not specified (nullptr). You can only set it by using the RouteBusstopItemData::setData() or RouteBusstopItemData::mergeData() method.
     * @param parent The QObject-parent
     * @param id The optional ID to be used. It can't be changed later.
     * @param isClone Specifies if the object is created as a clone.
     */
    explicit RouteBusstopItem(QObject *parent, const QUuid &id = QUuid(), const bool &isClone = false);

    /**
     * @brief Constructs a new RouteBusstopItem object by parsing a JSON object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param jsonObject The JSON object to parse.
     */
    explicit RouteBusstopItem(QObject *parent, const QJsonObject &jsonObject);

public:
    /**
     * @brief Returns the RouteBusstopItem's Busstop.
     *
     * @return The RouteBusstopItem's Busstop.
     */
    Busstop *busstop() const;

    /**
     * @brief Returns the RouteBusstopItem's default BusstopPlatform
     * @return The default platform
     */
    BusstopPlatform *defaultPlatform() const;

    /**
     * @brief Changes the RouteBusstopItem's default BusstopPlatform.
     * @param platform The new platform to set
     */
    void setDefaultPlatform(BusstopPlatform *platform);

    QJsonObject toJson() const override;

protected:
    void fromJson(const QJsonObject &jsonObject) override;
};

#endif // ROUTEBUSSTOPITEM_H
