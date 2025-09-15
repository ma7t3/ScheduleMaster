#ifndef ROUTE_H
#define ROUTE_H

#include "RouteBusstopItem.h"
#include "ProjectDataItemList.h"

class LineDirection;

/**
 * @struct RouteData
 * @brief The RouteData class contains the actual data of a Route object.
 *
 * It is seperated from the class logic to make it easier to change or completly replace it.
 */
struct RouteData : ProjectDataItemData<RouteData> {
    /// Constructs a new RouteData object. It should always call ProjectDataItemData::initParentOwnsItemMembers().
    RouteData() {initParentOwnsItemMembers();}

    QList<ProjectDataItemContainer *> parentOwnsItemsMembersList() override {return {&busstops};}

    /// The Route's name
    QString name;

    /// The Route's code
    int code = 0;

    /**
     * @brief The Route's LineDirection
     * This can be nullptr if the Route has no LineDirection assigned.
     *
     * **Important Note:** Always use the Route::direction() method to get the LineDirection.
     * This pointer may point to a platform that is not part of the Line's direction list.
     */
    LineDirection *direction = nullptr;

    /// The Route's busstops
    PDIList<RouteBusstopItem> busstops;
};

/**
 * @class Route
 * @brief The Route class represents one route.
 *
 * A route is a list of busstops that are visited in a certain order.
 * It also has a name and and a numeric code which is used to sort the routes in a useful order.
 * In addition, it can have multiple TimeProfiles. One TimeProfile describes how long the bus needs to travel from one busstop to the next.
 * Every route is assigned to one LineDirection as well but it doesn't take ownership of the direction since multiple routes can be assigned to the same direction.
 */

class Route : public ProjectDataItem<Route, RouteData> {
    friend class Line;
    friend class ProjectDataItem<Route, RouteData>;
    Q_OBJECT
private:
    /**
     * @brief Constructs a new Route object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param id The optional ID to be used. It can't be changed later.
     * @param isClone Specifies if the object is created as a clone.
     */
    explicit Route(QObject *parent, const QUuid &id = QUuid(), const bool &isClone = false);

    /**
     * @brief Constructs a new Route object by parsing a JSON object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param jsonObject The JSON object to parse.
     */
    explicit Route(QObject *parent, const QJsonObject &jsonObject);

public:
    /**
     * @brief Compares this Route with other by their names.
     * @param other
     * @return Whether this Route's name is smaller than the other's name.
     */
    bool operator<(const Route &other) const;

    /**
     * @brief Returns the Route's name.
     *
     * See also setName().
     * @return The Route's name
     */
    QString name() const;

    /**
     * @brief Replaces the Route's name.
     * @param newName The new name
     *
     * See also name().
     */
    void setName(const QString &newName);

    /**
     * @brief Returns the Route's code.
     *
     * See also code().
     * @return The Route's code
     */
    int code() const;

    /**
     * @brief Replaces the Route's code.
     * @param newCode The new code
     *
     * See also setCode().
     */
    void setCode(const int &newCode);

    /**
     * @brief Returns the Route's LineDirection
     *
     * **Note:** This can be nullptr if no LineDirection was assigned.
     * See also setDirection().
     * @return The Route's LineDirection.
     */
    LineDirection *direction() const;

    /**
     * @brief Changes the Route's LineDirection.
     * @param newDirection The new LineDirection
     *
     * If newDirection is nullptr or not inUse, this function does nothing.
     * See also direction().
     */
    void setDirection(LineDirection *newDirection);

    /**
     * @brief Creates a new RouteBusstopItem with an optionally given parent.
     * @param parent The parent QObject of the RouteBusstopItem object. If not given, the Route will take ownership.
     * @return The created RouteBusstopItem object
     */
    RouteBusstopItem *createItem(Busstop *b, QObject *parent = nullptr);

    /**
     * @brief Creates a new RouteBusstopItem object based on the given QJsonObject.
     * @param jsonObject The QJsonObject to read the RouteBusstopItem data from
     * @return The created RouteBusstopItem object
     */
    RouteBusstopItem *createItem(const QJsonObject &jsonObject);

    int busstopCount() const;
    RouteBusstopItem *busstop(const QUuid &id) const;
    PDIList<RouteBusstopItem> busstops() const;
    bool containsBusstop(Busstop *busstop) const;
    bool containsBusstop(const QUuid &id) const;
    PDIList<RouteBusstopItem> itemsWithBusstop(Busstop *busstop) const;
    PDIList<RouteBusstopItem> itemsWithBusstop(const QUuid &id) const;
    RouteBusstopItem *firstBusstop() const;
    RouteBusstopItem *lastBusstop() const;
    PDIList<RouteBusstopItem> commonBusstops(Route *route, const bool &sameDefaultPlatform) const;
    Busstop *firstCommonBusstop(Route *route, const bool &sameDefaultPlatform = false) const;
    Busstop *lastCommonBusstop(Route *route, const bool &sameDefaultPlatform = false) const;

    int indexOfBusstop(RouteBusstopItem *busstopItem) const;

    void appendBusstop(RouteBusstopItem *busstop);
    void insertBusstop(const int &index, RouteBusstopItem *busstop);
    void removeBusstop(RouteBusstopItem *busstop);
    void removeBusstop(const QUuid &id);
    void removeBusstopAt(const int &index);

    QJsonObject toJson() const override;

protected:
    void fromJson(const QJsonObject &jsonObject) override;
};

#endif // ROUTE_H
