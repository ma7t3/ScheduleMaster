#ifndef LINETABLEMODEL_H
#define LINETABLEMODEL_H

#include <QAbstractItemModel>

#include "ProjectDataModels/UnorderedProjectDataRowModel.h"
#include "src/projectdata/ProjectData.h"
#include "src/projectdata/Line.h"

class LineTableModel : public UnorderedProjectDataRowModel<Line> {
    Q_OBJECT

public:
    explicit LineTableModel(QObject *parent = nullptr);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;


protected:
    virtual PDISet<Line> fetch() const override;

private:
    ProjectData *_projectData;
};

#endif // LINETABLEMODEL_H
