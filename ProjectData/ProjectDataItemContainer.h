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
 * In addition to QList and QHash they provide some additional features like automatically updating the item's inUse property when items are added or removed.
 * It can also clone all it's items.
 */

class ProjectDataItemContainer {
public:

    /**
     * @brief Constructs a new ProjectDataItemContainer with both members set to false at first.
     */
    ProjectDataItemContainer() : _parentOwnsItems(false) {}

    virtual ~ProjectDataItemContainer() = default;

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
     * @brief Clones the container and all it's items.
     * @return A deep copy of the container.
     */
    virtual ProjectDataItemContainer *clone() const = 0;

    /**
     * @brief Creates clones (deep copies) of all items in the container and replaces the items.
     *
     * So the original items won't be part of the container anymore. You should always save them before or call this function only on clones.
     */
    virtual void cloneItems() = 0;

    /**
     * @brief Merges another container into this one.
     *
     * In contrast to replaceItems(), this function doesn't just replace the item pointers, instead it makes an ID based comparison and merges the data.
     * But it doesn't change the pointers at all.
     *
     * This is a virtual function that is implemented by the subclassed PDIList and PDISet.
     * @param mergeContainer The container to merge from
     */
    virtual void mergeItems(ProjectDataItemContainer *mergeContainer) = 0;

    /**
     * @brief Write a debug output of the container's items to the console.
     *
     * This is a virtual function that is implemented by the subclassed PDIList and PDISet.
     */
    virtual void dumpData() const = 0;

    /**
     * @brief Replaces all items in the container with the items of the otherContainer.
     *
     * In contrast to mergeItems(), this function just takes the literal item pointers from the otherContainer.
     *
     * This is a virtual function that is implemented by the subclassed PDIList and PDISet.
     * @param otherContainer The container to replace the items with
     */
    virtual void replaceItems(ProjectDataItemContainer *otherContainer) = 0;

    /**
     * @brief Clears the container completely
     *
     * This is a virtual function that is implemented by the subclassed PDIList and PDISet.
     */
    virtual void clear() = 0;

private:
    /**
     * @brief The _parentOwnsItems property saves, if the ProjectDataItemData class that contains this container takes ownership of the items in the container.
     *
     * See also parentOwnsItems() and setParentOwnsItems().
     */
    bool _parentOwnsItems;
};

#endif // PROJECTDATAITEMCONTAINER_H
