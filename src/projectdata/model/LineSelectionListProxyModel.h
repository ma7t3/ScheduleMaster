#ifndef LINESELECTIONLISTPROXYMODEL_H
#define LINESELECTIONLISTPROXYMODEL_H

#include <QSortFilterProxyModel>

class LineSelectionListProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit LineSelectionListProxyModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

};

#endif // LINESELECTIONLISTPROXYMODEL_H
