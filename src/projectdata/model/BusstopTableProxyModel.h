#ifndef BUSSTOPTABLEPROXYMODEL_H
#define BUSSTOPTABLEPROXYMODEL_H

#include "projectdata/model/SortFilterProxyModel.h"
#include "ui/widgets/FilterPopups/WdgBusstopsFilterPopup.h"

class BusstopTableProxyModel : public SortFilterProxyModel<WdgBusstopsFilterPopup> {
    Q_OBJECT
public:
    explicit BusstopTableProxyModel(QAbstractButton *popupButton, QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    ProjectData *_projectData;
};

#endif // BUSSTOPTABLEPROXYMODEL_H
