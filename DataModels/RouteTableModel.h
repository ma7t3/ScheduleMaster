#ifndef ROUTETABLEMODEL_H
#define ROUTETABLEMODEL_H

#include <QAbstractItemModel>

#include "ProjectDataRowBasedTableModel.h"

class RouteTableModel : public ProjectDataRowBasedTableModel<Route> {
    Q_OBJECT

public:
    explicit RouteTableModel(QObject *parent);
    void     setLine(Line *);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int      columnCount(const QModelIndex &parent = QModelIndex())                           const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;

protected:
    QList<Route *> fetchData()              const override;
    bool           testSearchMatch(Route *) const override;

private:
    Line *_line;
};

#endif // ROUTETABLEMODEL_H
