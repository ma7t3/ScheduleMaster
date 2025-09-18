#ifndef ROUTETABLEPROXYMODEL_H
#define ROUTETABLEPROXYMODEL_H

#include <QObject>
#include "ProjectDataModels/SortFilterProxyModel.h"

class RouteTableProxyModel : public SortFilterProxyModel {
    Q_OBJECT
public:
    explicit RouteTableProxyModel(QObject *parent = nullptr);

    bool lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif // ROUTETABLEPROXYMODEL_H
