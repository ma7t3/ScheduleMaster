#ifndef LINEDIRECTIONLISTMODEL_H
#define LINEDIRECTIONLISTMODEL_H

#include <QAbstractItemModel>

#include "DataModels/OrderedProjectDataRowModel.h"

class LineDirectionListModel : public OrderedProjectDataRowModel<LineDirection> {
    Q_OBJECT

public:
    explicit LineDirectionListModel(QObject *parent);
    void     setLine(Line *);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int      columnCount(const QModelIndex &parent = QModelIndex())                           const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;

protected:
    QList<LineDirection *> fetchData() const override;

private:
    Line *_line;
};

#endif // LINEDIRECTIONLISTMODEL_H
