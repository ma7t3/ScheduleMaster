#ifndef LINEDIRECTION_H
#define LINEDIRECTION_H

#include "ProjectDataItem.h"
#include "ProjectDataItemData.h"

/**
 * @struct LineDirectionData
 * @brief The LineDirectionData class contains the actual data of a LineDirection object.
 *
 * It is seperated from the class logic to make it easier to change or completly replace it.
 */
struct LineDirectionData : ProjectDataItemData<LineDirectionData> {
    LineDirectionData() {initParentOwnsItemMembers();}

    QList<ProjectDataItemContainer *> parentOwnsItemsMembersList() override {return {};}

    /// The LineDirection's description
    QString description;
};

/**
 * @class LineDirection
 * @brief The LineDirection class represents a travel direction inside a line.
 *
 * Every line can have multiple LineDirections (usually 1 or 2) which specify, which travel directions the Line has.
 * Every Line's Route has one specific direction it is connected to.
 */
class LineDirection : public ProjectDataItem<LineDirection, LineDirectionData> {
    Q_OBJECT
public:
    /**
     * @brief Constructs a new LineDirection object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param id The optional ID to be used. It can't be changed later.
     */
    explicit LineDirection(QObject *parent, const QUuid &id = QUuid(), const bool &isClone = false);

    /**
     * @brief Constructs a new LineDirection object by parsing a JSON object.
     *
     * If no ID was specified a new ID is generated.
     * @param parent The QObject-parent
     * @param jsonObject The JSON object to parse.
     */
    explicit LineDirection(QObject *parent, const QJsonObject &);

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

    QJsonObject toJson() const override;

protected:
    void fromJson(const QJsonObject &jsonObject) override;
};

#endif // LINEDIRECTION_H
