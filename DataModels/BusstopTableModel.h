#ifndef BUSSTOPTABLEMODEL_H
#define BUSSTOPTABLEMODEL_H

#include <QAbstractTableModel>

#include "DataModels/UnorderedProjectDataRowModel.h"

class BusstopTableModel : public UnorderedProjectDataRowModel<Busstop> {
    Q_OBJECT

public:
    explicit BusstopTableModel(QObject *parent);
    void     setProjectData(ProjectData *);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int      columnCount(const QModelIndex &parent = QModelIndex())                           const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;

protected:
    QList<Busstop *> fetchData()                            const override;
    QPixmap          generateLinesPixmap(Busstop *)         const;
    QString          generateUsedLinesSortString(Busstop *) const;

private:
    ProjectData *projectData;
};

#endif // BUSSTOPTABLEMODEL_H
