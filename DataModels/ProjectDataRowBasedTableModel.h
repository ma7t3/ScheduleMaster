#ifndef PROJECTDATAROWBASEDTABLEMODEL_H
#define PROJECTDATAROWBASEDTABLEMODEL_H

#include "DataModels/ProjectDataTableModel.h"
#include "ProjectData/projectdata.h"

template <typename T>
class ProjectDataRowBasedTableModel : public ProjectDataTableModel<T> {
public:
    explicit ProjectDataRowBasedTableModel(QObject *parent) : ProjectDataTableModel<T>(parent) {}

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override = 0;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        if (parent.isValid())
            return 0;

        return ProjectDataTableModel<T>::_items.count();
    }

    virtual int      columnCount(const QModelIndex &parent = QModelIndex())     const override = 0;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override = 0;
    virtual T *      itemAt(const QModelIndex &index)                           const override {return itemAt(index.row());}
    virtual T *      itemAt(const int &row)                                     const override {return ProjectDataTableModel<T>::_items[row];}

public slots:
    void setSearchFilter(const QString &search) override {
        const int oldLength = ProjectDataTableModel<T>::_search.length();
        const int newLength = search.length();
        ProjectDataTableModel<T>::_search = search;
        if(newLength < oldLength) {
            // add
            QList<T *> allItems = ProjectData::sortItems(fetchData());
            for(T *t : allItems) {
                if(testSearchMatch(t) && ProjectDataTableModel<T>::_items.indexOf(t) == -1) {
                    QList<T *> addedList = ProjectData::sortItems(ProjectDataTableModel<T>::_items << t);
                    int index = addedList.indexOf(t);
                    QAbstractTableModel::beginInsertRows(QModelIndex(), index, index);
                    ProjectDataTableModel<T>::_items = addedList;
                    QAbstractTableModel::endInsertRows();
                }
            }
        } else if(newLength > oldLength) {
            // remove
            for(int i = 0; i < ProjectDataTableModel<T>::_items.count(); i++) {
                T *t = ProjectDataTableModel<T>::_items[i];
                if(!testSearchMatch(t)) {
                    QAbstractTableModel::beginRemoveRows(QModelIndex(), i, i);
                    ProjectDataTableModel<T>::_items.remove(i);
                    QAbstractTableModel::endRemoveRows();
                    i--;
                }
            }
        }
    }

protected slots:
    void addItems(const QList<T *> list) override {
        QList<T *> allItems = ProjectDataTableModel<T>::_items;
        QList<T *> sortedList = ProjectData::sortItems(list);
        for(T *t : sortedList) {
            //insert and sort
            allItems << t;
            allItems = ProjectData::sortItems(allItems);

            // determine index
            const int index = allItems.indexOf(t);

            // do actual opperation
            QAbstractTableModel::beginInsertRows(QModelIndex(), index, index);
            ProjectDataTableModel<T>::_items = allItems;
            QAbstractTableModel::endInsertRows();
        }

        emit ProjectDataTableSignals::updateFinished();
    }

    void changeItems(const QList<T *> list) override {
        QList<T *> allItems = ProjectData::sortItems(ProjectDataTableModel<T>::_items);
        QList<T *> sortedList = ProjectData::sortItems(list);

        for(T *t : sortedList) {
            int oldIndex = ProjectDataTableModel<T>::_items.indexOf(t);
            int newIndex = allItems.indexOf(t);

            if(oldIndex == newIndex) {
                emit QAbstractTableModel::dataChanged(QAbstractTableModel::index(oldIndex, 0), QAbstractTableModel::index(oldIndex, 1));
                continue;
            }

            int newTableIndex = newIndex;
            if(newIndex > oldIndex) {
                newTableIndex++;
            }

            QAbstractTableModel::beginMoveRows(QModelIndex(), oldIndex, oldIndex, QModelIndex(), newTableIndex);
            ProjectDataTableModel<T>::_items.remove(oldIndex);
            ProjectDataTableModel<T>::_items.insert(newIndex, t);
            QAbstractTableModel::endMoveRows();
        }

        emit ProjectDataTableSignals::updateFinished();
    }

    void removeItems(const QList<T *> list) override {
        QList<T *> sortedList = ProjectData::sortItems(list);
        for(T *t : sortedList) {
            const int index = ProjectDataTableModel<T>::_items.indexOf(t);
            QAbstractTableModel::beginRemoveRows(QModelIndex(), index, index);
            ProjectDataTableModel<T>::_items.remove(index);
            QAbstractTableModel::endRemoveRows();
        }

        emit ProjectDataTableSignals::updateFinished();
    }

    void reset() override {
        QAbstractTableModel::beginResetModel();
        ProjectDataTableModel<T>::_items.clear();
        QAbstractTableModel::endResetModel();

        emit ProjectDataTableSignals::updateFinished();
    }

protected:
    virtual QList<T *> fetchData()    const override = 0;
    virtual bool testSearchMatch(T *) const override = 0;
};

#endif // PROJECTDATAROWBASEDTABLEMODEL_H
