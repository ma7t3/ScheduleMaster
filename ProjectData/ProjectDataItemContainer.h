#ifndef PROJECTDATAITEMCONTAINER_H
#define PROJECTDATAITEMCONTAINER_H

/**
 * @class ProjectDataItemContainer
 * @brief The ProjectDataItemContainer class is a general parent class for storing ProjectDataItems.
 *
 * Itself doesn't provide any container functionality, so it is only useful if inherited together with container class providet by Qt like QList or QHash (what PDIList and PDISet do).
 * This class just declares two boolean members that are used to track in which context the container is used and how it should behave when changing it's items.
 *
 * ProjectDataItemContainers (respectively their derived classes) should primarily be used insde the ProjectDataItemData classes (resp. their derived classes).
 *
 * ProjectDataItemContainers shouldn't change their context.
 * That is why the ProjectDataItemContainer's assigment operator is implicitly deleted. This ensures that the members can not not overwritten.
 *
 * Because of the general code architecture and limits of C++ it's effectively not possible to set the correct member values through the constuctor directly.
 * So the constructor sets them to false as a fallback to avoid undefined behavior.
 *
 * Later, right after construction but before the container is filled with data, the actual values should be set using the setter methods. But then later, they shouldn't be changed anymore.
 */

class ProjectDataItemContainer {
public:

    /**
     * @brief Constructs a new ProjectDataItemContainer with both members set to false at first.
     */
    ProjectDataItemContainer() : _parentOwnsItems(false), _isInClone(false) {}

    ProjectDataItemContainer &operator=(const ProjectDataItemContainer &other) = delete;

    /**
     * @brief Returns if the _parentOwnsItems property is set to true.
     * @return The value of _parentOwnsItems
     */
    bool parentOwnsItems() const {
        return _parentOwnsItems;
    }

    /**
     * @brief Changes the _parentOwnsItems property.
     *
     * **Attention:** You should only set this value once right after creating the list but later never again.
     * @param parentOwnsItems The new value to set.
     */
    void setParentOwnsItems(const bool &parentOwnsItems) {
        _parentOwnsItems = parentOwnsItems;
    }

    /**
     * @brief Returns if the _isInClone property is set to true.
     * @return The value of _isInClone
     */
    bool isInClone() const {
        return _isInClone;
    }

    /**
     * @brief Changes the _isInClone property.
     *
     * **Attention:** You should only set this value once right after creating the list but later never again.
     * @param parentOwnsItems The new value to set.
     */
    void setIsInClone(const bool &isInClone) {
        _isInClone = isInClone;
    }

    /**
     * @brief Returns if the container should update the item's inUse property or not.
     *
     * It will return true if _parentOwnsItems is true and _isInClone is false.
     * @return Whether the container should update the item's inUse property or not.
     */
    bool shouldUpdateInUse() const {
        return _parentOwnsItems && !_isInClone;
    }

protected:
    /**
     * @brief The _parentOwnsItems property saves, if the ProjectDataItemData class that contains this container takes ownership of the items in the container.
     *
     * See also parentOwnsItems() and setParentOwnsItems().
     */
    bool _parentOwnsItems;

    /**
     * @brief The _isInClone property saves, if the ProjectDataItemData class that contains this container (resp. the ProjectDataItem that contains the Data object with the container) is a cloned version and not the "original".
     *
     * See also isInClone() and setIsInClone().
     */
    bool _isInClone;
};

#endif // PROJECTDATAITEMCONTAINER_H
