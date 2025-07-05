#ifndef ROUTETABLEPROXYMODEL_H
#define ROUTETABLEPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

class RouteTableProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit RouteTableProxyModel(QObject *parent = nullptr);

    bool lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const override;
};

#endif // ROUTETABLEPROXYMODEL_H
