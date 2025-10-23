#ifndef PROJECTDATAITEMLIST_H
#define PROJECTDATAITEMLIST_H

#include "ProjectDataItemContainer.h"

#include <QList>
#include <QUuid>
#include <QMap>
#include <QDebug>

/**
 * @class ProjectDataItemList
 * @brief The ProjectDataItemList class is container for storing ProjectDataItems in a defined order.
 *
 * ProjectDataItemList inherits from QList and mostly behaves exactly like a QList with some additional features.
 * For example, it provides better find and filter functions as well as supoorts the isUsed property provided by ProjectDataItem.
 *
 * Due to to the architecture of this class, lists cannot contain the same item multiple times if parentOwnsItems() is set to true.
 *
 * See also ProjectDataItemSet. It mostly behaves exactly the same but it stores it's items without a defined order.
 */

template <typename T>
class ProjectDataItemList : public QList<T *>, public ProjectDataItemContainer {
public:
    /**
     * @brief Constructs a new and empty ProjectDataItemList
     */
    ProjectDataItemList() : QList<T *>() {};

    ProjectDataItemContainer *clone() const override {
        return new ProjectDataItemList(*this);
    };

    void cloneItems(QObject *parent) override {
        for(int i = 0; i < this->count(); i++)
            this->replace(i, this->at(i)->clone(parent));
    }

    void duplicateItems(QObject *parent) override {
        for(int i = 0; i < this->count(); i++)
            this->replace(i, this->at(i)->duplicate(parent));
    }

    void mergeItems(ProjectDataItemContainer *mergeContainer, QObject *parent) override {
        ProjectDataItemList *otherList = dynamic_cast<ProjectDataItemList *>(mergeContainer);
        if(!otherList)
            return;

        // remove
        for(int i = this->count() - 1; i >= 0; --i)
            if(!otherList->contains(this->at(i)->id()))
                this->removeAt(i);

        // update
        for(T *current : *this)
            if(otherList->contains(current->id())) {
                current->mergeData(otherList->find(current->id())->data());
                current->setParent(parent);
            }

        // add
        for(T *current : *otherList)
            if(!contains(current->id())) {
                append(current);
                current->setParent(parent);
            }

        // reorder
        QMap<QUuid, int> indexMap;
        for (int i = 0; i < otherList->size(); ++i)
            indexMap.insert(otherList->at(i)->id(), i);

        std::sort(this->begin(), this->end(), [&](const auto &a, const auto &b) {return indexMap[a->id()] < indexMap[b->id()];});
    }

    void dumpData() const override {
        int i = 0;
        for(T *current : *this) {
            qDebug().noquote() << i << current->dump();
            i++;
        }
    }

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
    void append(T *item) {
        if(!item || (parentOwnsItems() && contains(item->id())))
            return;

        QList<T *>::append(item);
        if(parentOwnsItems())
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
    void insert(const int &index, T *item) {
        if(index < 0 || index > this->count() || !item || (parentOwnsItems() && contains(item->id())))
            return;

        QList<T *>::insert(index, item);
        if(parentOwnsItems())
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
        for(T *current : *this)
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
        for(T *current : *this)
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
    void remove(T* item) {
        QList<T *>::removeAll(item);
        if(parentOwnsItems())
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
    void remove(const QUuid &id) {
        int i = 0;
        while(i < this->count()) {
            if(this->value(i)->id() == id)
                removeAt(i);
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
    void removeAt(const int &index) {
        if(index < 0 || index >= this->count())
            return;

        T *current = this->value(index);
        QList<T *>::remove(index);
        if(parentOwnsItems())
            current->setInUse(false);
    }

    void replaceItems(ProjectDataItemContainer *otherContainer) override {
        ProjectDataItemList *otherList = static_cast<ProjectDataItemList *>(otherContainer);
        clear();
        for(T *current : *otherList)
            append(current);
    }

    /**
     * @brief Clears the entire list (i.e. removes all items)
     * @param updateInUse If set to true, all item's inUse property will be set to true.
     */
    void clear() override {
        if(parentOwnsItems())
            for(T *current : *this)
                current->setInUse(false);

        QList<T *>::clear();
    }

    template<typename R, typename Function>
    QList<R> map(Function f) {
        QList<R> result;
        for(T *item : *this)
            result.append(f(item));
        return result;
    }

    QJsonArray toJson() const override {
        QJsonArray result;
        for(T *item : *this) {
            result.append(item->toJson());
        }
        return result;
    }
};

template <typename T>
using PDIList = ProjectDataItemList<T>;

#endif // PROJECTDATAITEMLIST_H
