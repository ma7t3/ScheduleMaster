#ifndef PROJECTDATAITEMLIST_H
#define PROJECTDATAITEMLIST_H

#include <QList>
#include <QUuid>

/**
 * @class ProjectDataItemList
 * @brief The ProjectDataItemList class is container for storing ProjectDataItems in a defined order.
 *
 * ProjectDataItemList inherits from QList and mostly behaves exactly like a QList with some additional features.
 * For example, it provides better find and filter functions as well as supoorts the isUsed property provided by ProjectDataItem.
 *
 * See also ProjectDataItemSet. It mostly behaves exactly the same but it stores it's items without a defined order.
 */

template <typename T>
class ProjectDataItemList : public QList<T *> {
public:
    /**
     * @brief Constructs a new and empty ProjectDataItemList
     */
    ProjectDataItemList() : QList<T *>() {};

    /**
     * @brief Checks if the ProjectDataItem with the id is part of the list.
     * @param id The id to search for
     * @return Whether an item with a matching id was found or not.
     */
    bool contains(const QUuid &id) const {
        return find(id) != nullptr;
    }

    /**
     * @brief Checks if the ProjectDataItem is part of the list.
     * @param item The item to search for
     * @return Whether the item was found or not.
     */
    bool contains(T *item) const {
        return QList<T *>::contains(item);
    }

    /**
     * @brief Searches for an item with the id within the list and returns it.
     *
     * If there was not item found that matches the id, nullptr will be returned.
     * @param id The id to search for
     * @return The item that was found or nullptr if no item was found.
     */
    T *find(const QUuid &id) const {
        auto it = std::find_if(this->begin(), this->end(),
                               [id](T *current) {
            return current->id() == id;
        });

        return it != this->end() ? *it : nullptr;
    }

    /**
     * @brief Appends a new item to the end of the list.
     *
     * If the item is nullptr, it won't be added.
     * @param item The item to add
     * @param updateInUse If set to true, the item's inUse property will be set to true.
     */
    void append(T *item, const bool &updateInUse = false) {
        if(!item)
            return;

        QList<T *>::append(item);
        if(updateInUse)
            item->setInUse(true);
    }


    /**
     * @brief Inserts a new item at a certain position at the end of the list.
     *
     * The item will not be inserted if it is nullptr or the index is out of range.
     * @param index The index to insert at
     * @param item The item to insert
     * @param updateInUse If set to true, the item's inUse property will be set to true.
     */
    void insert(const int &index, T *item, const bool &updateInUse = false) {
        QList<T *>::insert(index, item);
        if(updateInUse)
            item->setInUse(true);
    }

    /**
     * @brief Returns a filtered list which will only contains items that match the filter specified with filter.
     *
     * The filter has to be a function that takes a pointer of type T* which will point to one item of the list.
     * The function should return true or false depending if the item matches the filter or not.
     * @param filter The filter function
     * @return The filtered list
     */
    template <typename Filter>
    ProjectDataItemList<T> filter(Filter filter) const {
        ProjectDataItemList<T> result;
        for(T *current : *this)
            if(filter(current))
                result.append(current);

        return result;
    }

    /**
     * @brief Works exactly the same like ProjectDataItemList::filter() but it will only return the first item in the list that matches the given filter.
     *
     * This is useful for filters where you expect to get only one item that matches the filter.
     * Returns nullptr if no item was found.
     *
     * The filter has to be a function that takes a pointer of type T* which will point to one item of the list.
     * The function should return true or false depending if the item matches the filter or not.
     * @param filter The filter function.
     * @return The first item that matched the filter or nullptr if no matching item was found.
     */
    template <typename Filter>
    T *filterOne(Filter filter) const {
        for(T *current : *this)
            if(filter(current))
                return current;

        return nullptr;
    }

    /**
     * @brief Checks if at least one item in the list matches a certain filter.
     *
     * The filter has to be a function that takes a pointer of type T* which will point to one item of the list.
     * The function should return true or false depending if the item matches the filter or not.
     * @param filter The filter function
     * @return True if at least one item matched the filter, else false.
     */
    template <typename Filter>
    bool some(Filter filter) const {
        for(T current : *this)
            if(filter(current))
                return true;

        return false;
    }

    /**
     * @brief Checks if every item in the list matches a certain filter.
     *
     * The filter has to be a function that takes a pointer of type T* which will point to one item of the list.
     * The function should return true or false depending if the item matches the filter or not.
     * @param filter The filter function
     * @return True if every item matched the filter, else false.
     */
    template <typename Filter>
    bool every(Filter filter) const {
        for(T current : *this)
            if(!filter(current))
                return false;

        return true;
    }

    /**
     * @brief Removes an item from the list.
     *
     * If the item is not part of the list, this function does nothing.
     * @param item The item to remove
     * @param updateInUse If set to true, the item's inUse property will be set to true.
     */
    void remove(T* item, const bool &updateInUse = false) {
        QList<T *>::removeAll(item);
        if(updateInUse)
            item->setInUse(false);
    }

    /**
     * @brief Removes an item specified by an id from the list.
     *
     * If the item is not part of the list, this function does nothing.
     *
     * If the same item (with the same id) occurs multiple times in the list, every occurence will be removed.
     * @param id The id of the item to remove
     * @param updateInUse If set to true, the item's inUse property will be set to true.
     */
    void remove(const QUuid &id, const bool &updateInUse = false) {
        int i = 0;
        while(i < this->count()) {
            if(this->value(i)->id() == id)
                removeAt(i, updateInUse);
            else
                i++;
        }
    }

    /**
     * @brief Removes an item at a certain position from the list.
     *
     * If the index is out of range, this function does nothing.
     * @param item The index of the item to remove
     * @param updateInUse If set to true, the item's inUse property will be set to true.
     */
    void removeAt(const int &index, const bool &updateInUse = false) {
        if(index < 0 || index >= this->count())
            return;

        T *current = this->value(index);
        QList<T *>::remove(index);
        if(updateInUse)
            current->setInUse(false);
    }

    /**
     * @brief Clears the entire list (i.e. removes all items)
     * @param updateInUse If set to true, all item's inUse property will be set to true.
     */
    void clear(const bool &updateInUse = false) {
        if(updateInUse)
            for(T current : *this)
                current->setInUse(false);

        QList<T *>::clear();
    }
};

template <typename T>
using PDIList = ProjectDataItemList<T>;

#endif // PROJECTDATAITEMLIST_H
