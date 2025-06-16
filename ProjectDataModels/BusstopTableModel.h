#ifndef BUSSTOPTABLEMODEL_H
#define BUSSTOPTABLEMODEL_H

#include <QAbstractItemModel>

#include "ProjectDataModels/UnorderedProjectDataRowModel.h"
#include "ProjectData/Busstop.h"
#include "ProjectData/ProjectData.h"

class BusstopTableModel : public UnorderedProjectDataRowModel<Busstop> {
    Q_OBJECT

public:
    explicit BusstopTableModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    virtual PDISet<Busstop> fetch() const override;

private:
    ProjectData *_projectData;
};

#endif // BUSSTOPTABLEMODEL_H
