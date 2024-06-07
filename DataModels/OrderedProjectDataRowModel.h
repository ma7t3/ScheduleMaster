#ifndef ORDEREDPROJECTDATAROWMODEL_H
#define ORDEREDPROJECTDATAROWMODEL_H

#include "DataModels/ProjectDataModel.h"
#include "ProjectData/projectdata.h"

template <typename T>
class OrderedProjectDataRowModel : public ProjectDataModel<T> {
public:
    explicit OrderedProjectDataRowModel(QObject *parent) : ProjectDataModel<T>(parent) {}

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override = 0;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        if (parent.isValid())
            return 0;

        return ProjectDataModel<T>::_items.count();
    }

    virtual int      columnCount(const QModelIndex &parent = QModelIndex())     const override = 0;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override = 0;

public slots:
    void setSearchFilter(const QString &search) override {
        // TODO: reimplement
        /*
        const int oldLength = ProjectDataModel<T>::_search.length();
        const int newLength = search.length();
        ProjectDataModel<T>::_search = search;
        if(newLength < oldLength) {
            // add
            QList<T *> allItems = fetchData();
            for(T *t : allItems) {
                if(testSearchMatch(t) && ProjectDataModel<T>::_items.indexOf(t) == -1) {
                    QList<T *> addedList = ProjectData::sortItems(ProjectDataModel<T>::_items << t);
                    int index = addedList.indexOf(t);
                    QAbstractTableModel::beginInsertRows(QModelIndex(), index, index);
                    ProjectDataModel<T>::_items = addedList;
                    QAbstractTableModel::endInsertRows();
                }
            }
        } else if(newLength > oldLength) {
            // remove
            for(int i = 0; i < ProjectDataModel<T>::_items.count(); i++) {
                T *t = ProjectDataModel<T>::_items[i];
                if(!testSearchMatch(t)) {
                    QAbstractTableModel::beginRemoveRows(QModelIndex(), i, i);
                    ProjectDataModel<T>::_items.remove(i);
                    QAbstractTableModel::endRemoveRows();
                    i--;
                }
            }
        }*/
    }

protected slots:
    void addItems(QList<T *> addList) override {
        QList<T *> newItems = fetchData();
        std::sort(addList.begin(), addList.end(), [newItems](T *a, T *b){return newItems.indexOf(a) < newItems.indexOf(b);});

        for(T *t : addList) {
            // find index
            const int index = newItems.indexOf(t);

            // do actual opperation
            QAbstractTableModel::beginInsertRows(QModelIndex(), index, index);
            ProjectDataModel<T>::_items.insert(index, t);
            QAbstractTableModel::endInsertRows();
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
                emit QAbstractTableModel::dataChanged(QAbstractTableModel::index(oldIndex, 0), QAbstractTableModel::index(oldIndex, 1));
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

        emit ProjectDataModelSignals::updateFinished();
    }

    void removeItems(QList<T *> changelist) override {
        QList<T *> newItems = fetchData();
        std::sort(changelist.begin(), changelist.end(), [newItems](T *a, T *b){return newItems.indexOf(a) < newItems.indexOf(b);});

        for(T *t : changelist) {
            const int index = ProjectDataModel<T>::_items.indexOf(t);
            QAbstractTableModel::beginRemoveRows(QModelIndex(), index, index);
            ProjectDataModel<T>::_items.remove(index);
            QAbstractTableModel::endRemoveRows();
        }

        emit ProjectDataModelSignals::updateFinished();
    }

protected:
    virtual QList<T *> fetchData()    const override = 0;
    virtual bool testSearchMatch(T *) const override = 0;
};

#endif // ORDEREDPROJECTDATAROWMODEL_H
