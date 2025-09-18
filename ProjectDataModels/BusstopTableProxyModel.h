#ifndef BUSSTOPTABLEPROXYMODEL_H
#define BUSSTOPTABLEPROXYMODEL_H

#include "ProjectDataModels/SortFilterProxyModel.h"

class BusstopTableProxyModel : public SortFilterProxyModel {
    Q_OBJECT
public:
    explicit BusstopTableProxyModel(QObject *parent = nullptr);

    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif // BUSSTOPTABLEPROXYMODEL_H
