#ifndef LINEDIRECTION_H
#define LINEDIRECTION_H

#include "ProjectDataItem.h"

/**
 * @class LineDirection
 * @brief The LineDirection class represents a travel direction inside a line.
 *
 * Every line can have multiple LineDirections (usually 1 or 2) which specify, which travel directions the Line has.
 * Every Line's Route has one specific direction it is connected to.
 */
class LineDirection : public ProjectDataItem {
    Q_OBJECT
public:
    /**
     * @brief Constructs a new LineDirection object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param id The optional ID to be used. It can't be changed later.
     */
    explicit LineDirection(QObject *parent, const QUuid &id = QUuid());

    /**
     * @brief Constructs a new LineDirection object by parsing a JSON object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param jsonObject The JSON object to parse.
     */
    explicit LineDirection(QObject *parent, const QJsonObject &);

    /**
     * @struct LineDirection::Data
     * @brief The LineDirection::Data class contains the actual data of a LineDirection object.
     *
     * It is seperated from the class logic to make it easier to change or completly replace it.
     */
    struct Data {
        /// The LineDirection's description
        QString description;
    };

    /**
     * @brief Returns the LineDirection's data.
     *
     * See also setData().
     * @return The LineDirection's data.
     */
    Data data() const;

    /**
     * @brief Replaces the LineDirection's data.
     * @param newData The new data
     *
     * See also data().
     */
    void setData(const Data &newData);

    /**
     * @brief Returns the LineDirection's description.
     * @return The LineDirection's description
     */
    QString description() const;

    /**
     * @brief Replaces the LineDirection's description.
     * @param newDescription The new description
     */
    void setDescription(const QString &newDescription);

    /**
     * @brief Converts the Direction to a JSON object
     *
     * See also fromJson().
     * @return the QJsonObject
     */
    QJsonObject toJson() const;

protected:

    /**
     * @brief Reads a JSON object and fills the Direction with the data from the JSON object.
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
    /// The LineDirection's data
    LineDirection::Data _data;
};

#endif // LINEDIRECTION_H
