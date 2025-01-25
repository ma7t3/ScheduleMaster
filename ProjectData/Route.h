#ifndef ROUTE_H
#define ROUTE_H

#include "LineDirection.h"


/**
 * @class Route
 * @brief The Route class represents one route.
 *
 * A route is a list of busstops that are visited in a certain order.
 * It also has a name and and a numeric code which is used to sort the routes in a useful order.
 * In addition, it can have multiple TimeProfiles. One TimeProfile describes how long the bus needs to travel from one busstop to the next.
 * Every route is assigned to one LineDirection as well but it doesn't take ownership of the direction since multiple routes can be assigned to the same direction.
 */

class Route : public ProjectDataItem {
    Q_OBJECT
public:
    /**
     * @brief Constructs a new Route object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param id The optional ID to be used. It can't be changed later.
     */
    explicit Route(QObject *parent, const QUuid &id = QUuid());

    /**
     * @brief Constructs a new Route object by parsing a JSON object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param jsonObject The JSON object to parse.
     */
    explicit Route(QObject *parent, const QJsonObject &jsonObject);

    /**
     * @brief Compares this Route with other by their names.
     * @param other
     * @return Whether this Route's name is smaller than the other's name.
     */
    bool operator<(const Route &other) const;

    /**
     * @struct Route::Data
     * @brief The Route::Data class contains the actual data of a Route object.
     *
     * It is seperated from the class logic to make it easier to change or completly replace it.
     */
    struct Data {
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
    };

    /**
     * @brief Returns the Route's data.
     *
     * See also setData().
     * @return The Route's data.
     */
    Data data() const;

    /**
     * @brief Replaces the Route's data.
     * @param newData The new data
     *
     * See also data().
     */
    void setData(const Data &newData);

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

    QJsonObject toJson() const override;

protected:
    void fromJson(const QJsonObject &jsonObject) override;

private:
    /// The Route's data.
    Route::Data _data;
};

#endif // ROUTE_H
