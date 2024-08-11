#ifndef DAYTYPELISTMODEL_H
#define DAYTYPELISTMODEL_H

#include <QAbstractItemModel>

#include "DataModels/OrderedProjectDataRowModel.h"

class DayTypeListModel : public OrderedProjectDataRowModel<DayType> {
    Q_OBJECT
public:
    explicit DayTypeListModel(QObject *parent);
    void     setProjectData(ProjectData *);
    void     setProjectSettings(ProjectSettings *);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int      columnCount(const QModelIndex &parent = QModelIndex())                           const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;

protected:
    QList<DayType *> fetchData() const override;

private:
    ProjectData *_projectData;
    ProjectSettings *_projectSettings;

    // sets mode (true = take DayTypes from ProjectData, false = take DayTypes from ProjectSettings)
    bool _mode;
};

#endif // DAYTYPELISTMODEL_H
