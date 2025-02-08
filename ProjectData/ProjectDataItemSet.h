#ifndef PROJECTDATAITEMSET_H
#define PROJECTDATAITEMSET_H

#include <QHash>
#include <QUuid>

#include "ProjectDataItemContainer.h"

/**
 * @class ProjectDataItemSet
 * @brief The ProjectDataItemSet class is container for storing ProjectDataItems without a defined order.
 *
 * ProjectDataItemSet inherits from QHash and mostly behaves exactly like a QHash with some additional features.
 * For example, it provides better find and filter functions as well as supoorts the isUsed property provided by ProjectDataItem.
 *
 * See also ProjectDataItemList. It mostly behaves exactly the same but it stores it's items in a defined order.
 */

template <typename T>
class ProjectDataItemSet : public QHash<QUuid, T *>, public ProjectDataItemContainer {
public:
    /**
     * @brief Constructs a new and empty ProjectDataItemSet
     */
    ProjectDataItemSet() : QHash<QUuid, T *>() {};

    void cloneItems() override {
        for(T *item : *this)
            QHash<QUuid, T* >::insert(item->id(), item->clone());
    }

    void dumpData() const  {
        for(T *current : this->values()) {
            qDebug().noquote() << current->id() << current->dump();
        }
    }

    /**
     * @brief Checks if the ProjectDataItem with the id is part of the set.
     * @param id The id to search for
     * @return Whether an item with a matching id was found or not.
     */
    bool contains(const QUuid &id) const {
        return find(id) != nullptr;
    }

    /**
     * @brief Checks if the ProjectDataItem is part of the set.
     * @param item The item to search for
     * @return Whether the item was found or not.
     */
    bool contains(T *item) const {
        return QHash<QUuid, T *>::values().contains(item);
    }

    /**
     * @brief Searches for an item with the id within the set and returns it.
     *
     * If there was not item found that matches the id, nullptr will be returned.
     * @param id The id to search for
     * @return The item that was found or nullptr if no item was found.
     */
    T *find(const QUuid &id) const {
        return QHash<QUuid, T *>::contains(id) ? QHash<QUuid, T *>::value(id) : nullptr;
    }

    /**
     * @brief Adds a new item to the set.
     *
     * If the item is nullptr or it already is in the set, this function does nothing.
     * @param item The item to add
     * @param updateInUse If set to true, the item's inUse property will be set to true.
     */
    void add(T *item) {
        if(!item || contains(item))
            return;

        QHash<QUuid, T *>::insert(item->id(), item);
        if(parentOwnsItems())
            item->setInUse(true);
    }

    /**
     * @brief Returns a filtered set which will only contains items that match the filter specified with filter.
     *
     * The filter has to be a function that takes a pointer of type T* which will point to one item of the set.
     * The function should return true or false depending if the item matches the filter or not.
     * @param filter The filter function
     * @return The filtered set
     */
    template <typename Filter>
    ProjectDataItemSet<T> filter(Filter filter) const {
        ProjectDataItemSet<T> result;
        for(T *current : *this) {
            if(filter(current))
                result.add(current);
        }
        return result;
    }

    /**
     * @brief Works exactly the same like ProjectDataItemSet::filter() but it will only return the first item in the set that matches the given filter.
     *
     * This is useful for filters where you expect to get only one item that matches the filter.
     * Returns nullptr if no item was found.
     *
     * The filter has to be a function that takes a pointer of type T* which will point to one item of the set.
     * The function should return true or false depending if the item matches the filter or not.
     * @param filter The filter function.
     * @return The first item that matched the filter or nullptr if no matching item was found.
     */
    template <typename Filter>
    T *filterOne(Filter filter) const {
        for(T *current : *this) {
            if(filter(current))
                return current;
        }
        return nullptr;
    }

    /**
     * @brief Checks if at least one item in the set matches a certain filter.
     *
     * The filter has to be a function that takes a pointer of type T* which will point to one item of the set.
     * The function should return true or false depending if the item matches the filter or not.
     * @param filter The filter function
     * @return True if at least one item matched the filter, else false.
     */
    template <typename Filter>
    bool some(Filter filter) const {
        for(T *current : *this)
            if(filter(current))
                return true;

        return false;
    }

    /**
     * @brief Checks if every item in the set matches a certain filter.
     *
     * The filter has to be a function that takes a pointer of type T* which will point to one item of the set.
     * The function should return true or false depending if the item matches the filter or not.
     * @param filter The filter function
     * @return True if every item matched the filter, else false.
     */
    template <typename Filter>
    bool every(Filter filter) const {
        for(T *current : *this)
            if(!filter(current))
                return false;

        return true;
    }

    /**
     * @brief Removes an item from the set.
     *
     * If the item is not part of the set, this function does nothing.
     * @param item The item to remove
     * @param updateInUse If set to true, the item's inUse property will be set to true.
     */
    void remove(T *item) {
        if(parentOwnsItems())
            item->setInUse(false);
        remove(item->id());
    }

    /**
     * @brief Removes an item specified by an id from the set.
     *
     * If the item is not part of the set, this function does nothing.
     *
     * If the same item (with the same id) occurs multiple times in the set, every occurence will be removed.
     * @param id The id of the item to remove
     * @param updateInUse If set to true, the item's inUse property will be set to true.
     */
    void remove(const QUuid &id) {
        T *item = QHash<QUuid, T *>::value(id);
        if(item && parentOwnsItems())
            item->setInUse(false);

        QHash<QUuid, T *>::remove(id);
    }

    void replaceItems(ProjectDataItemContainer *otherContainer) override {
        ProjectDataItemSet *otherSet = static_cast<ProjectDataItemSet *>(otherContainer);
        clear();
        for(T *current : *otherSet)
            add(current);
    }

    /**
     * @brief Clears the entire set (i.e. removes all items)
     * @param updateInUse If set to true, all item's inUse property will be set to true.
     */
    void clear() override {
        if(parentOwnsItems())
            for(T *current : *this)
                current->setInUse(false);

        QHash<QUuid, T *>::clear();
    }
};

template <typename T>
using PDISet = ProjectDataItemSet<T>;

#endif // PROJECTDATAITEMSET_H
