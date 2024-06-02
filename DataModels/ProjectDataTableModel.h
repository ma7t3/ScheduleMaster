#ifndef PROJECTDATATABLEMODEL_H
#define PROJECTDATATABLEMODEL_H

#include <QAbstractTableModel>

#include "DataModels/ProjectDataTableSignals.h"

template<typename T>
class ProjectDataTableModel : public ProjectDataTableSignals {
public:
    explicit ProjectDataTableModel(QObject *parent) : ProjectDataTableSignals(parent) {}
    virtual  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override = 0;
    virtual  int rowCount(const QModelIndex &parent = QModelIndex())                                   const override = 0;
    virtual  int columnCount(const QModelIndex &parent = QModelIndex())                                const override = 0;
    virtual  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override = 0;
             int      itemCount()                                                                      const          {return ProjectDataTableModel<T>::_items.count(); }
             T *      itemAt(const QModelIndex &index)                                                 const          {return itemAt(index.row());}
             T *      itemAt(const int &row)                                                           const          {return ProjectDataTableModel<T>::_items[row];}

public slots:
    virtual void setSearchFilter(const QString &) = 0;
    virtual void addItems(const QList<T *>)       = 0;
    virtual void changeItems(const QList<T *>)    = 0;
    virtual void removeItems(const QList<T *>)    = 0;
    virtual void reset() = 0;

protected:
    virtual QList<T *> fetchData()    const = 0;
    virtual bool testSearchMatch(T *) const = 0;

    QList<T *> _items;
    QString    _search;
};

#endif // PROJECTDATATABLEMODEL_H
