#ifndef PROJECTDATAMODEL_H
#define PROJECTDATAMODEL_H

#include <QAbstractTableModel>

#include "DataModels/ProjectDataModelSignals.h"

template<typename T>
class ProjectDataModel : public ProjectDataModelSignals {
public:
    explicit ProjectDataModel(QObject *parent) : ProjectDataModelSignals(parent) {}
    virtual  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override = 0;
    virtual  int rowCount(const QModelIndex &parent = QModelIndex())                                   const override = 0;
    virtual  int columnCount(const QModelIndex &parent = QModelIndex())                                const override = 0;
    virtual  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override = 0;
             int        itemCount()                                                                    const          {return ProjectDataModel<T>::_items.count(); }
             // T *        itemAt(const QModelIndex &index)                                               const          {return itemAt(index.row());}
             T *        itemAt(const int &index)                                                       const          {return ProjectDataModel<T>::_items[index];}
             QList<T *> itemList()                                                                     const          {return _items;}

public slots:
    virtual void setSearchFilter(const QString &) = 0;
    virtual void addItems(const QList<T *>)       = 0;
    virtual void changeItems(const QList<T *>)    = 0;
    virtual void removeItems(const QList<T *>)    = 0;
    void reset() {
        QAbstractTableModel::beginResetModel();
        ProjectDataModel<T>::_items.clear();
        ProjectDataModel<T>::_items = fetchData();
        QAbstractTableModel::endResetModel();
        emit ProjectDataModelSignals::updateFinished();
    }

protected:
    virtual QList<T *> fetchData() const = 0;

    virtual bool testSearchMatch(T *) const = 0;

    QList<T *> _items;
    QString    _search;
};

#endif // PROJECTDATAMODEL_H
