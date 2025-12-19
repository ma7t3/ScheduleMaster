#ifndef TIMEPROFILETABLEMODEL_H
#define TIMEPROFILETABLEMODEL_H

#include "src/projectdata/model/OrderedProjectDataRowModel.h"
#include "src/projectdata/Route.h"

class TimeProfileTableModel : public OrderedProjectDataRowModel<TimeProfile> {
    Q_OBJECT
public:
    explicit TimeProfileTableModel(QObject *parent = nullptr);

    void setRoute(Route *route);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    virtual PDIList<TimeProfile> fetch() const override;

private:
    Route *_route;
};

#endif // TIMEPROFILETABLEMODEL_H
