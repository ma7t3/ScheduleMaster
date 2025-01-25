#ifndef ROUTEBUSSTOPITEM_H
#define ROUTEBUSSTOPITEM_H

#include "Busstop.h"
#include "BusstopPlatform.h"

class RouteBusstopItem : public ProjectDataItem {
    Q_OBJECT
public:
    /**
     * @brief Constructs a new RouteBusstopItem object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param busstop The busstop to be used in this RouteBusstopItem
     * @param id The optional ID to be used. It can't be changed later.
     */
    explicit RouteBusstopItem(QObject *parent, Busstop *busstop, const QUuid &id = QUuid());

    /**
     * @brief Constructs a new RouteBusstopItem object by parsing a JSON object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param jsonObject The JSON object to parse.
     */
    explicit RouteBusstopItem(QObject *parent, const QJsonObject &jsonObject);

    /**
     * @struct RouteBusstopItem::Data
     * @brief The RouteBusstopItem::Data class contains the actual data of a RouteBusstopItem object.
     *
     * It is seperated from the class logic to make it easier to change or completly replace it.
     */
    struct Data {
        /// The RouteBusstopItem's Busstop
        Busstop *busstop = nullptr;

        /// The busstop's defaultPlatform in this RouteBusstopItem
        BusstopPlatform *defaultPlatform = nullptr;
    };

    /**
     * @brief Returns the RouteBusstopItem's data.
     *
     * See also setData().
     * @return The RouteBusstopItem's data.
     */
    Data data() const;

    /**
     * @brief Replaces the RouteBusstopItem's data.
     * @param newData The new data
     *
     * See also data().
     */
    void setData(const Data &newData);

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

private:
    /// The RouteBusstopItem's data.
    RouteBusstopItem::Data _data;
};

#endif // ROUTEBUSSTOPITEM_H
