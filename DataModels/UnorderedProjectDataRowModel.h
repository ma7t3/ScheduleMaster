#ifndef UNORDEREDPROJECTDATAROWMODEL_H
#define UNORDEREDPROJECTDATAROWMODEL_H

#include "DataModels/ProjectDataModel.h"
#include "ProjectData/projectdata.h"

template <typename T>
class UnorderedProjectDataRowModel : public ProjectDataModel<T> {
public:
    explicit UnorderedProjectDataRowModel(QObject *parent) : ProjectDataModel<T>(parent) {}

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override = 0;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        if (parent.isValid())
            return 0;

        return ProjectDataModel<T>::_items.count();
    }

    virtual int      columnCount(const QModelIndex &parent = QModelIndex())     const override = 0;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override = 0;

protected slots:
    void addItems(const QList<T *> list) override {
        ProjectDataModel<T>::_updating = true;
        QList<T *> allItems = ProjectDataModel<T>::_items;
        QList<T *> sortedList = ProjectData::sortItems(list);
        for(T *t : sortedList) {
            //insert and sort
            allItems << t;
            allItems = ProjectData::sortItems(allItems);

            // determine index
            const int index = allItems.indexOf(t);

            // do actual opperation
            QAbstractTableModel::beginInsertRows(QModelIndex(), index, index);
            ProjectDataModel<T>::_items = allItems;
            QAbstractTableModel::endInsertRows();
        }

        ProjectDataModel<T>::_updating = false;
        emit ProjectDataModelSignals::updateFinished();
    }

    void changeItems(const QList<T *> list) override {
        ProjectDataModel<T>::_updating = true;
        QList<T *> allItems = ProjectData::sortItems(ProjectDataModel<T>::_items);
        QList<T *> sortedList = ProjectData::sortItems(list);

        for(T *t : sortedList) {
            int oldIndex = ProjectDataModel<T>::_items.indexOf(t);
            int newIndex = allItems.indexOf(t);

            if(oldIndex == newIndex) {
                emit QAbstractTableModel::dataChanged(QAbstractTableModel::index(oldIndex, 0), QAbstractTableModel::index(oldIndex, columnCount() - 1));
                continue;
            }

            int newTableIndex = newIndex;
            if(newIndex > oldIndex) {
                newTableIndex++;
            }

            QAbstractTableModel::beginMoveRows(QModelIndex(), oldIndex, oldIndex, QModelIndex(), newTableIndex);
            ProjectDataModel<T>::_items.remove(oldIndex);
            ProjectDataModel<T>::_items.insert(newIndex, t);
            QAbstractTableModel::endMoveRows();
        }

        ProjectDataModel<T>::_updating = false;
        emit ProjectDataModelSignals::updateFinished();
    }

    void removeItems(const QList<T *> list) override {
        ProjectDataModel<T>::_updating = true;
        QList<T *> sortedList = ProjectData::sortItems(list);
        for(T *t : sortedList) {
            const int index = ProjectDataModel<T>::_items.indexOf(t);
            QAbstractTableModel::beginRemoveRows(QModelIndex(), index, index);
            ProjectDataModel<T>::_items.remove(index);
            QAbstractTableModel::endRemoveRows();
        }

        ProjectDataModel<T>::_updating = false;
        emit ProjectDataModelSignals::updateFinished();
    }

protected:
    virtual QList<T *> fetchData()    const override = 0;
};

#endif // UNORDEREDPROJECTDATAROWMODEL_H
