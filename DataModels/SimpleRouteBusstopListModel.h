#ifndef SIMPLEROUTEBUSSTOPLISTMODEL_H
#define SIMPLEROUTEBUSSTOPLISTMODEL_H

#include <QAbstractItemModel>

#include "DataModels/OrderedProjectDataRowModel.h"

class SimpleRouteBusstopListModel : public OrderedProjectDataRowModel<Busstop> {
    Q_OBJECT

public:
    explicit SimpleRouteBusstopListModel(QObject *parent);
    void setRoute(Route *);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {Q_UNUSED(section);Q_UNUSED(orientation);Q_UNUSED(role);return QVariant();};
    int      columnCount(const QModelIndex &parent = QModelIndex())                           const override {Q_UNUSED(parent);return 1;};
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;

protected:
    QList<Busstop *> fetchData() const override;

private:
    Route *_route;
};

#endif // SIMPLEROUTEBUSSTOPLISTMODEL_H
