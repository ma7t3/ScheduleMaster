#ifndef TIMEPROFILETABLEMODEL_H
#define TIMEPROFILETABLEMODEL_H

#include <QAbstractItemModel>

#include "DataModels/OrderedProjectDataRowModel.h"

class TimeProfileTableModel : public OrderedProjectDataRowModel<TimeProfile> {
    Q_OBJECT

public:
    explicit TimeProfileTableModel(QObject *parent);
    void setRoute(Route *);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int      columnCount(const QModelIndex &parent = QModelIndex())                           const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;

protected:
    QList<TimeProfile *> fetchData()                    const override;

private:
    Route *_route;
};

#endif // TIMEPROFILETABLEMODEL_H
