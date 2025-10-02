#ifndef LINETABLEPROXYMODEL_H
#define LINETABLEPROXYMODEL_H

#include <QObject>
#include "ProjectDataModels/SortFilterProxyModel.h"
#include "Widgets/FilterPopups/WdgBusstopsFilterPopup.h"

class LineTableProxyModel : public SortFilterProxyModel<WdgBusstopsFilterPopup> {
    Q_OBJECT
public:
    explicit LineTableProxyModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif // LINETABLEPROXYMODEL_H
