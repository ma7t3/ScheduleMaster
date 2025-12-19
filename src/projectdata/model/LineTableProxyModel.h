#ifndef LINETABLEPROXYMODEL_H
#define LINETABLEPROXYMODEL_H

#include <QObject>
#include "src/projectdata/model/SortFilterProxyModel.h"
#include "src/ui/widgets/FilterPopups/WdgLineFilterPopup.h"

class LineTableProxyModel : public SortFilterProxyModel<WdgLineFilterPopup> {
    Q_OBJECT
public:
    explicit LineTableProxyModel(QAbstractButton *popupButton, QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif // LINETABLEPROXYMODEL_H
