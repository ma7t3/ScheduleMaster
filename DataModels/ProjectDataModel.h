#ifndef PROJECTDATAMODEL_H
#define PROJECTDATAMODEL_H

#include <QAbstractTableModel>

#include "DataModels/ProjectDataModelSignals.h"

template<typename T>
class ProjectDataModel : public ProjectDataModelSignals {
public:
    explicit ProjectDataModel(QObject *parent) : ProjectDataModelSignals(parent), _updating(false) {}
    virtual  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override = 0;
    virtual  int rowCount(const QModelIndex &parent = QModelIndex())                                   const override = 0;
    virtual  int columnCount(const QModelIndex &parent = QModelIndex())                                const override = 0;
    virtual  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override = 0;
             int        indexOf(T *t)                                                                  const          {return _items.indexOf(t);}
             int        itemCount()                                                                    const          {return ProjectDataModel<T>::_items.count(); }
             T *        itemAt(const int &index)                                                       const          {if(index < 0 || index >= itemCount()) return nullptr; else return ProjectDataModel<T>::_items[index];}
             QList<T *> itemList()                                                                     const          {return _items;}
             bool       updating()                                                                     const          {return _updating;}

public slots:
    virtual void addItems       (const QList<T *>) = 0;
    virtual void changeItems    (const QList<T *>) = 0;
    virtual void removeItems    (const QList<T *>) = 0;
    virtual void reset() {
        _updating = true;
        QAbstractTableModel::beginResetModel();
        _items.clear();
        _items = fetchData();
        QAbstractTableModel::endResetModel();
        _updating = false;
        emit ProjectDataModelSignals::updateFinished();
    }

protected:
    virtual QList<T *> fetchData() const = 0;

    QList<T *> _items;
    bool _updating;
    int _dataFieldsCount;
};

#endif // PROJECTDATAMODEL_H
