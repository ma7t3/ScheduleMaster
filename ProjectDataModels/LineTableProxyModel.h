#ifndef LINETABLEPROXYMODEL_H
#define LINETABLEPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

class LineTableProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit LineTableProxyModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif // LINETABLEPROXYMODEL_H
