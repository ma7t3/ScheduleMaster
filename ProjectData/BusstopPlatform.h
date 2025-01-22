#ifndef BUSSTOPPLATFORM_H
#define BUSSTOPPLATFORM_H

#include "ProjectDataItem.h"


/**
 * @class BusstopPlatform
 * @brief The BusstopPlatform class represents a platform that belongs to a busstop.
 *
 * One busstop has a name and certain flags that describe its features.
 */

class BusstopPlatform : public ProjectDataItem {
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

    /**
     * @struct BusstopPlatform::Data
     * @brief The BusstopPlatform::Data class contains the actual data of a BusstopPlatform object.
     *
     * It is seperated from the class logic to make it easier to change or completly replace it.
     */
    struct Data {
        /// The BusstopPlatform's name
        QString name;

        /// The BusstopPlatform's flags
        BusstopPlatformFlags flags;
    };

    /**
     * @brief Returns the BusstopPlatform's data.
     *
     * See also setData().
     * @return The Busstop's data.
     */
    Data data() const;

    /**
     * @brief Replaces the BusstopPlatform's data.
     * @param data The new data
     *
     * See also data().
     */
    void setData(const Data &);

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

    /**
     * @brief Converts the BusstopPlatform to a JSON object
     *
     * See also fromJson().
     * @return the QJsonObject
     */
    QJsonObject toJson() const;

protected:
    /**
     * @brief Reads a JSON object and fills the BusstopPlatform with the data from the JSON object.
     *
     * See also toJson().
     * @param jsonObject The QJsonObject to read from.
     */
    void fromJson(const QJsonObject &jsonObject);

signals:
    /**
     * @brief This signal is emited whenever the _data is changed.
     *
     * This can be just one data property to be changed or the entire data object being replaced.
     */
    void changed();

private:
    /// The BusstopPlatform's data.
    BusstopPlatform::Data _data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(BusstopPlatform::BusstopPlatformFlags)

#endif // BUSSTOPPLATFORM_H
