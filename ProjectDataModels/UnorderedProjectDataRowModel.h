#ifndef UNORDEREDPROJECTDATAROWMODEL_H
#define UNORDEREDPROJECTDATAROWMODEL_H

#include <QUuid>
#include <QTimer>

#include "ProjectDataModels/UnorderedProjectDataRowModelSignals.h"

#include "ProjectData/ProjectDataItemSet.h"

/**
 * @class UnorderedProjectDataRowModel
 * @brief A model handling a set of unordered project data items within a table view.
 *
 * This template class provides an interface to manage a collection of items of type T
 * in an unordered fashion. It inherits from QAbstractTableModel to support Qt's Model/View
 * architecture and provides a general way to react to changes in the underlying data.
 *
 * @tparam T The type of items managed by the model.
 */
template<typename T>
class UnorderedProjectDataRowModel : public UnorderedProjectDataRowModelSignals {

public:
    /**
     * @brief Constructs the model with an optional parent.
     * @param parent The parent QObject for memory management.
     */
    explicit UnorderedProjectDataRowModel(QObject *parent = nullptr) :
        UnorderedProjectDataRowModelSignals(parent), _multipleRowsInserted(false) {}

    /**
     * @brief Returns the index for the specified row and column.
     * @param row The row of the item.
     * @param column The column of the item.
     * @param parent The parent index (default is QModelIndex()).
     * @return The QModelIndex for the specified row and column.
     */
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return createIndex(row, column, itemAt(row));
    }

    /**
     * @brief Returns the number of rows in the model.
     * @param parent The parent index (default is QModelIndex()).
     * @return The number of rows.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        if(parent.isValid())
            return 0;

        return _data.size();
    }

    /**
     * @brief Retrieves the item at the specified index.
     * @param index The index of the item.
     * @return A pointer to the item at the specified index.
     */
    T *itemAt(const int &index) const {
        if(index >= 0 && index < _cache.count())
            return _cache[index];
        else
            return nullptr;
    }

    /**
     * @brief Finds the index of the specified item.
     * @param item The item to find.
     * @return The index of the item, or -1 if not found.
     */
    int indexOfItem(T *item) const {
        return _cache.indexOf(item);
    }

    /**
     * @brief Returns all items currently shown in the model as a list.
     * @return The list of all currently shown items
     */
    QList<T *> itemList() const {
        return _data.values();
    }

    /**
     * @brief Resets the model, clearing and reloading all data.
     *
     * This function clears the current data, fetches new data, and updates the model.
     */
    void reset() {
        beginResetModel();
        for(T *item : _data)
            unregisterForUpdate(item);

        _connections.clear(); // for safety to avoid "lost connections"

        PDISet<T> hash = fetch();
        _data = hash.toMap();

        for(T *item : _data)
            registerForUpdate(item);

        invalidateCache();
        endResetModel();
    }

protected:
    /**
     * @brief Fetches the data set to be managed by the model.
     * @return A set of project data items.
     */
    virtual PDISet<T> fetch() const = 0;

    /**
     * @brief Invalidates and rebuilds the cache of items.
     */
    void invalidateCache() {
        _cache = _data.values();
    }

    /**
     * @brief Adds a new item to the model.
     * @param item The item to be added.
     */
    void onItemAdded(T *item) {
        if(_data.contains(item->id()))
            return;

        const auto it = _data.lowerBound(item->id());
        const int row = std::distance(_data.begin(), it);

        beginInsertRows(QModelIndex(), row, row);
        _data.insert(item->id(), item);
        _cache.insert(row, item);
        registerForUpdate(item);
        _insertedIndexes << QPersistentModelIndex(index(row, 0));

        if(!_multipleRowsInserted) {
            QTimer::singleShot(1, this, &UnorderedProjectDataRowModel::processInsertedIndexes);
            _multipleRowsInserted = true;
        }

        endInsertRows();
    }

    /**
     * @brief Updates the specified item in the model.
     * @param item The item to be updated.
     */
    void onItemUpdated(T *item) {
        if(!_data.contains(item->id()))
            return;

        const int row = _cache.indexOf(item);
        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
    }

    /**
     * @brief Removes the specified item from the model.
     * @param item The item to be removed.
     */
    void onItemRemoved(T *item) {
        if(!_data.contains(item->id()))
            return;

        const auto it = _data.find(item->id());
        const int index = std::distance(_data.begin(), it);

        beginRemoveRows(QModelIndex(), index, index);
        _data.remove(item->id());
        _cache.remove(index);
        unregisterForUpdate(item);
        endRemoveRows();
    }

    /**
     * @brief Registers an item for update notifications.
     * @param item The item to register.
     */
    void registerForUpdate(T *item) {
        _connections.insert(item, connect(item, &T::changed, this, [this, item]() { onItemUpdated(item); }));
    }

    /**
     * @brief Unregisters an item from update notifications.
     * @param item The item to unregister.
     */
    void unregisterForUpdate(T *item) {
        disconnect(_connections.value(item));
        _connections.remove(item);
    }

    /**
     * @brief Processes all inserted indexes within the last QEventLoopRun and emits a collective event.
     */
    void processInsertedIndexes() {
        emit multipleRowsInserted(_insertedIndexes);
        _insertedIndexes.clear();
        _multipleRowsInserted = false;
    }

private:
    QMap<QUuid, T*> _data; ///< Map of data items identified by UUIDs.
    QList<T *> _cache; ///< Cached list of data items for fast access.
    QHash<T *, QMetaObject::Connection> _connections; ///< Connections for item updates.
    QList<QPersistentModelIndex> _insertedIndexes; ///< Collection of all rows that where inserted during the current QEventLoopRun
    bool _multipleRowsInserted; ///< Flag to indicate if multiple rows were inserted during the current QEventLoopRun
};

#endif // UNORDEREDPROJECTDATAROWMODEL_H
