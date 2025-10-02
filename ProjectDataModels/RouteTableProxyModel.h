#ifndef ROUTETABLEPROXYMODEL_H
#define ROUTETABLEPROXYMODEL_H

#include <QObject>
#include "ProjectDataModels/SortFilterProxyModel.h"
#include "Widgets/FilterPopups/WdgBusstopsFilterPopup.h"

class RouteTableProxyModel : public SortFilterProxyModel<WdgBusstopsFilterPopup> {
    Q_OBJECT
public:
    explicit RouteTableProxyModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif // ROUTETABLEPROXYMODEL_H
