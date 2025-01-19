#ifndef LINETABLEMODEL_H
#define LINETABLEMODEL_H

#include "DataModels/UnorderedProjectDataRowModel.h"

class LineTableModel : public UnorderedProjectDataRowModel<Line> {
    Q_OBJECT

public:
    explicit LineTableModel(QObject *parent);
    void     setProjectData(ProjectData *);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int      columnCount(const QModelIndex &parent = QModelIndex())                           const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;

protected:
    QList<Line *> fetchData() const override;

    void refreshDefaultSortIndexes();

private:
    ProjectData *projectData;
    QMap<Line *, int> _defaultSortIndexes;

    bool _refreshSorting;
};

#endif // LINETABLEMODEL_H
