#ifndef SIMPLECUSTOMBUSSTOPLISTMODEL_H
#define SIMPLECUSTOMBUSSTOPLISTMODEL_H

#include <QAbstractItemModel>

#include "DataModels/ProjectDataRowBasedTableModel.h"

class SimpleCustomBusstopListModel : public ProjectDataRowBasedTableModel<Busstop> {
    Q_OBJECT

public:
    explicit SimpleCustomBusstopListModel(QObject *parent);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {Q_UNUSED(section);Q_UNUSED(orientation);Q_UNUSED(role);return QVariant();};
    int      columnCount(const QModelIndex &parent = QModelIndex())                           const override {Q_UNUSED(parent);return 1;};
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;

    void addBusstop(Busstop *, const int &index = -1);
    void removeBusstop(Busstop *);
    void removeBusstop(const int &index);
    void setBusstops(QList<Busstop *>);

protected:
    QList<Busstop *> fetchData()                const override {return {};};
    bool             testSearchMatch(Busstop *) const override {return true;};

private:
    ProjectData *projectData;
};

#endif // SIMPLECUSTOMBUSSTOPLISTMODEL_H
