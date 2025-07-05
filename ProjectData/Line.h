#ifndef LINE_H
#define LINE_H

#include "LineDirection.h"
#include "ProjectDataItemList.h"

#include <QColor>

/**
 * @struct LineData
 * @brief The LineData class contains the actual data of a Line object.
 *
 * It is seperated from the class logic to make it easier to change or completly replace it.
 */
struct LineData : ProjectDataItemData<LineData> {
    /// Constructs a new LineData object. It should always call ProjectDataItemData::initParentOwnsItemMembers().
    LineData() {initParentOwnsItemMembers();}

    QList<ProjectDataItemContainer *> parentOwnsItemsMembersList() override {return {&directions};}

    /// The Line's name
    QString name;

    /// The Line's description
    QString description;

    /// The Line's color
    QColor color;

    /// The Line's directions
    PDIList<LineDirection> directions;
    // routes, trips
};

/**
 * @class Line
 * @brief The Line class represents one line.
 *
 * A line contains of multiple routes which represent a certain order of busstops.
 * All routes are categorized into directions.
 * Every line also has multiple trips which you can think of as instances of one route with a certain time.
 */

class Line : public ProjectDataItem<Line, LineData> {
    friend class ProjectData;
    friend class ProjectDataItem<Line, LineData>;
    Q_OBJECT
private:
    /**
     * @brief Constructs a new Line object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param id The optional ID to be used. It can't be changed later.
     * @param isClone Specifies if the object is created as a clone.
     */
    explicit Line(QObject *parent, const QUuid &id = QUuid(), const bool &isClone = false);

    /**
     * @brief Constructs a new Line object by parsing a JSON object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param jsonObject The JSON object to parse.
     */
    explicit Line(QObject *parent, const QJsonObject &jsonObject);

public:
    /**
     * @brief Compares this Line with other by their names.
     * @param other
     * @return Whether this Line's name is smaller than the other's name.
     */
    bool operator<(const Line &other) const;

    /**
     * @brief Returns the Line's name.
     *
     * See also setName().
     * @return The Line's name
     */
    QString name() const;

    /**
     * @brief Replaces the Line's name.
     * @param newName The new name
     *
     * See also name().
     */
    void setName(const QString &newName);

    /**
     * @brief Returns the Line's description.
     * @return The Line's description
     */
    QString description() const;

    /**
     * @brief Replaces the Line's description.
     * @param newDescription The new description
     */
    void setDescription(const QString &newDescription);

    /**
     * @brief Returns the Line's color.
     * @return The Line's color
     */
    QColor color() const;

    /**
     * @brief Replaces the Line's color.
     * @param newColor The new color
     */
    void setColor(const QColor &newColor);

    /**
     * @brief Creates a new direction with an optionally given parent.
     * @param parent The parent QObject of the LineDirection object. If not given, the Line will take ownership.
     * @return The created LineDirection object
     */
    LineDirection *createDirection(QObject *parent = nullptr);

    /**
     * @brief Creates a new LineDirection object based on the given QJsonObject.
     * @param jsonObject The QJsonObject to read the LineDirection data from
     * @return The created LineDirection object
     */
    LineDirection *createDirection(const QJsonObject &jsonObject);

    /**
     * @brief Returns the number of Directions in the Line.
     *
     * See also directions().
     * @return The number of busstops in the Line
     */
    int directionCount() const;

    /**
     * @brief Searches for a LineDirection by its UUID.
     * @param id The UUID of the Directions to search for.
     * @return A pointer to the Directions if it was found, otherwise nullptr.
     */
    LineDirection *direction(const QUuid &id) const;

    /**
     * @brief Returns a list of all Directions in the Line.
     *
     * See also directionCount().
     * @return A ProjectDataItemList of all Directions in the Line.
     */
    PDIList<LineDirection> directions() const;

    /**
     * @brief Returns the index of the given direction.
     * @param direction The LineDirection to search for
     * @return The index of the given LineDirection or -1 if not found
     */
    int indexOfDirection(LineDirection *direction) const;

    /**
     * @brief Appends a LineDirection to the end of the Line's LineDirection list.
     * @param direction The LineDirection to add.
     */
    void appendDirection(LineDirection *direction);

    /**
     * @brief Inserts a LineDirection at position index to the Line's LineDirection list.
     *
     * This function does nothing if the index is invalid.
     * @param index The index to be inserted at.
     * @param direction The LineDirection to add.
     */
    void insertDirection(const int &index, LineDirection *direction);

    /**
     * @brief Moves the direction at the given index to a new position.
     * @param from The current index of the LineDirection to move.
     * @param to The new index of the LineDirection to move.
     */
    void moveDirection(const int &from, const int &to);

    /**
     * @brief Moves the direction to a new position.
     * @param direction The LineDirection to move.
     * @param to The new index of the LineDirection to move.
     */
    void moveDirection(LineDirection *direction, const int &to);

    /**
     * @brief Removes a LineDirection from the Line. This does nothing if the given LineDirection is not part of the Line or is nullptr.
     * @param direction The LineDirection to remove.
     */
    void removeDirection(LineDirection *direction);

    /**
     * @brief Removes a LineDirection from the Line by its UUID. This does nothing if there was no LineDirection found that matches the given UUID.
     * @param id The LineDirection's id to search for.
     */
    void removeDirection(const QUuid &id);

    QJsonObject toJson() const override;

signals:
    void directionAdded(int row, LineDirection *);
    void directionRemoved(int row, LineDirection *);
    void directionChanged(int row, LineDirection *);
    void directionMoved(int from, int to);

protected:
    void fromJson(const QJsonObject &jsonObject) override;
};

#endif // LINE_H
