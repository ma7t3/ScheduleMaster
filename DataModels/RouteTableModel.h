#ifndef ROUTETABLEMODEL_H
#define ROUTETABLEMODEL_H

#include <QAbstractItemModel>

#include "UnorderedProjectDataRowModel.h"

class RouteTableModel : public UnorderedProjectDataRowModel<Route> {
    Q_OBJECT

public:
    explicit RouteTableModel(QObject *parent);
    void     setLine(Line *);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int      columnCount(const QModelIndex &parent = QModelIndex())                           const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;

protected:
    QList<Route *> fetchData() const override;
    void           refreshDefaultSortIndexes();

private:
    Line *_line;
    QMap<Route *, int> defaultSortIndexes;

    bool _refreshSorting;
};

#endif // ROUTETABLEMODEL_H
