#ifndef ORDEREDPROJECTDATACOLUMNMODEL_H
#define ORDEREDPROJECTDATACOLUMNMODEL_H

#include "DataModels/ProjectDataModel.h"
#include "ProjectData/projectdata.h"

template <typename T>
class OrderedProjectDataColumnModel : public ProjectDataModel<T> {
public:
    explicit OrderedProjectDataColumnModel(QObject *parent) : ProjectDataModel<T>(parent) {}

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override = 0;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override = 0;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        if(parent.isValid())
            return 0;

        return ProjectDataModel<T>::_items.count();
    }
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override = 0;

protected slots:
    void addItems(QList<T *> addList) override {
        QList<T *> newItems = fetchData();

        if(newItems.count() == ProjectDataModel<T>::_items.count()) {
            emit ProjectDataModelSignals::updateFinished();
            return;
        } else if(newItems.count() < ProjectDataModel<T>::_items.count()) {
            ProjectDataModel<T>::reset();
            return;
        }

        std::sort(addList.begin(), addList.end(), [newItems](T *a, T *b){return newItems.indexOf(a) < newItems.indexOf(b);});

        for(T *t : addList) {
            // find index
            const int index = newItems.indexOf(t);

            // do actual opperation
            QAbstractTableModel::beginInsertColumns(QModelIndex(), index, index);
            ProjectDataModel<T>::_items.insert(index, t);
            QAbstractTableModel::endInsertColumns();
        }

        emit ProjectDataModelSignals::updateFinished();
    }

    void changeItems(QList<T *> changelist) override {
        QList<T *> newItems = fetchData();
        std::sort(changelist.begin(), changelist.end(), [newItems](T *a, T *b){return newItems.indexOf(a) < newItems.indexOf(b);});

        for(T *t : changelist) {
            // find index
            const int oldIndex = ProjectDataModel<T>::_items.indexOf(t);
            const int newIndex = newItems.indexOf(t);

            if(oldIndex == newIndex) {
                emit QAbstractTableModel::dataChanged(QAbstractTableModel::index(0, oldIndex), QAbstractTableModel::index(_dataFieldsCount - 1, oldIndex));
                continue;
            }

            int newTableIndex = newIndex;
            if(newIndex > oldIndex) {
                newTableIndex++;
            }

            QAbstractTableModel::beginMoveColumns(QModelIndex(), oldIndex, oldIndex, QModelIndex(), newTableIndex);
            ProjectDataModel<T>::_items.remove(oldIndex);
            ProjectDataModel<T>::_items.insert(newIndex, t);
            QAbstractTableModel::endMoveColumns();
        }

        emit ProjectDataModelSignals::updateFinished();
    }

    void removeItems(QList<T *> changelist) override {
        QList<T *> newItems = fetchData();

        if(newItems.count() == ProjectDataModel<T>::_items.count()) {
            emit ProjectDataModelSignals::updateFinished();
            return;
        } else if(newItems.count() > ProjectDataModel<T>::_items.count()) {
            ProjectDataModel<T>::reset();
            return;
        }

        std::sort(changelist.begin(), changelist.end(), [newItems](T *a, T *b){return newItems.indexOf(a) < newItems.indexOf(b);});

        for(T *t : changelist) {
            const int index = ProjectDataModel<T>::_items.indexOf(t);
            QAbstractTableModel::beginRemoveColumns(QModelIndex(), index, index);
            ProjectDataModel<T>::_items.remove(index);
            QAbstractTableModel::endRemoveColumns();
        }

        emit ProjectDataModelSignals::updateFinished();
    }

protected:
    virtual QList<T *> fetchData() const override = 0;
    int _dataFieldsCount;
};

#endif // ORDEREDPROJECTDATACOLUMNMODEL_H
