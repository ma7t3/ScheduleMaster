#ifndef SIMPLEBUSSTOPLISTMODEL_H
#define SIMPLEBUSSTOPLISTMODEL_H

#include <QAbstractItemModel>

#include "DataModels/UnorderedProjectDataRowModel.h"

class SimpleBusstopListModel : public UnorderedProjectDataRowModel<Busstop> {
    Q_OBJECT

public:
    explicit SimpleBusstopListModel(QObject *parent);
    void     setProjectData(ProjectData *);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {Q_UNUSED(section);Q_UNUSED(orientation);Q_UNUSED(role);return QVariant();};
    int      columnCount(const QModelIndex &parent = QModelIndex())                           const override {Q_UNUSED(parent);return 1;};
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;

protected:
    QList<Busstop *> fetchData() const override;

private:
    ProjectData *projectData;
};

#endif // SIMPLEBUSSTOPLISTMODEL_H
