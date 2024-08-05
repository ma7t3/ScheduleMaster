#ifndef DAYTYPELISTMODEL_H
#define DAYTYPELISTMODEL_H

#include <QAbstractItemModel>

#include "DataModels/OrderedProjectDataRowModel.h"

class DayTypeListModel : public OrderedProjectDataRowModel<DayType> {
    Q_OBJECT
public:
    explicit DayTypeListModel(QObject *parent);
    void     setProjectData(ProjectData *);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int      columnCount(const QModelIndex &parent = QModelIndex())                           const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;

protected:
    QList<DayType *> fetchData() const override;

private:
    ProjectData *projectData;
};

#endif // DAYTYPELISTMODEL_H
