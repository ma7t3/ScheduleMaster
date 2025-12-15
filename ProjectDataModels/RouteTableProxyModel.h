#ifndef ROUTETABLEPROXYMODEL_H
#define ROUTETABLEPROXYMODEL_H

#include <QObject>
#include "ProjectDataModels/SortFilterProxyModel.h"
#include "src/ui/widgets/FilterPopups/WdgRouteFilterPopup.h"

class RouteTableProxyModel : public SortFilterProxyModel<WdgRouteFilterPopup> {
    Q_OBJECT
public:
    explicit RouteTableProxyModel(QAbstractButton *popupButton, QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif // ROUTETABLEPROXYMODEL_H
