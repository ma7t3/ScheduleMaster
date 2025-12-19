#ifndef ORDEREDPROJECTDATAROWMODEL_H
#define ORDEREDPROJECTDATAROWMODEL_H

#include "ProjectDataModels/UnorderedProjectDataRowModelSignals.h"

#include "src/projectdata/ProjectDataItemList.h"

#include <QTimer>

/**
 * @class OrderedProjectDataRowModel
 * @brief A model handling a set of ordered project data items within a table view.
 *
 * This template class provides an interface to manage a collection of items of type T
 * in an ordered fashion. It inherits from QAbstractTableModel to support Qt's Model/View
 * architecture and provides a general way to react to changes in the underlying data.
 *
 * @tparam T The type of items managed by the model.
 */
template<typename T>
class OrderedProjectDataRowModel : public UnorderedProjectDataRowModelSignals {

public:
    /**
     * @brief Constructs the model with an optional parent.
     * @param parent The parent QObject for memory management.
     */
    explicit OrderedProjectDataRowModel(QObject *parent = nullptr) :
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
        if(index >= 0 && index < _data.count())
            return _data[index];
        else
            return nullptr;
    }

    /**
     * @brief Finds the index of the specified item.
     * @param item The item to find.
     * @return The index of the item, or -1 if not found.
     */
    int indexOfItem(T *item) const {
        return _data.indexOf(item);
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

        _data = fetch();

        for(T *item : _data)
            registerForUpdate(item);

        endResetModel();
    }

    /**
     * @brief Inserts a new item to the model.
     * @param row The row at which to insert the item.
     * @param item The item to be added.
     */
    void onItemInserted(const int &row, T *item) {
        beginInsertRows(QModelIndex(), row, row);
        _data.insert(row, item);
        registerForUpdate(item);
        _insertedIndexes << QPersistentModelIndex(index(row, 0));

        if(!_multipleRowsInserted) {
            QTimer::singleShot(1, this, &OrderedProjectDataRowModel::processInsertedIndexes);
            _multipleRowsInserted = true;
        }

        endInsertRows();
    }

    /**
     * @brief Updates the specified item in the model.
     * @param row The row of the item to be updated.
     */
    void onItemUpdated(const int &row) {
        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
    }

    /**
     * @brief Moves an item to its new position
     * @param from The current index of the item to move.
     * @param to The new index of the item to move.
     */
    void onItemMoved(const int &from, const int &to) {
        if (from == to)
            return;

        const int dest = (to > from) ? to + 1 : to;

        beginMoveRows(QModelIndex(), from, from, QModelIndex(), dest);
        _data.insert(to, _data.takeAt(from));
        endMoveRows();
    }

    /**
     * @brief Removes the specified item from the model.
     * @param row The row of the item to be removed.
     * @param item The item to be removed.
     */
    void onItemRemoved(const int &row, T *item) {
        beginRemoveRows(QModelIndex(), row, row);
        _data.remove(row);
        unregisterForUpdate(item);
        endRemoveRows();
    }

    /**
     * @brief Registers an item for update notifications.
     * @param item The item to register.
     */
    void registerForUpdate(T *item) {
        _connections.insert(item, connect(item, &T::changed, this, [this, item]() { onItemUpdated(indexOfItem(item)); }));
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

protected:
    /**
     * @brief Fetches the data set to be managed by the model.
     * @return A set of project data items.
     */
    virtual PDIList<T> fetch() const = 0;

private:
    QList<T *> _data;
    QHash<T *, QMetaObject::Connection> _connections; ///< Connections for item updates.
    QList<QPersistentModelIndex> _insertedIndexes; ///< Collection of all rows that where inserted during the current QEventLoopRun
    bool _multipleRowsInserted; ///< Flag to indicate if multiple rows were inserted during the current QEventLoopRun
};

#endif // ORDEREDPROJECTDATAROWMODEL_H
