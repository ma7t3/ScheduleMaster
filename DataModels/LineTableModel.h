#ifndef LINETABLEMODEL_H
#define LINETABLEMODEL_H

#include "DataModels/ProjectDataRowBasedTableModel.h"

class LineTableModel : public ProjectDataRowBasedTableModel<Line> {
    Q_OBJECT

public:
    explicit LineTableModel(QObject *parent);
    void     setProjectData(ProjectData *);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int      columnCount(const QModelIndex &parent = QModelIndex())                           const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;

protected:
    QList<Line *> fetchData()             const override;
    bool          testSearchMatch(Line *) const override;

private:
    ProjectData *projectData;
};

#endif // LINETABLEMODEL_H
