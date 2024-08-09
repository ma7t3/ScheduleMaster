#ifndef ROUTELISTMODEL_H
#define ROUTELISTMODEL_H

#include <QObject>
#include "DataModels/UnorderedProjectDataRowModel.h"

class RouteListModel : public UnorderedProjectDataRowModel<Route> {
    Q_OBJECT
public:
    explicit RouteListModel(QObject *parent);
    void     setDirection(LineDirection *);

    QVariant      headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {Q_UNUSED(section);Q_UNUSED(orientation);Q_UNUSED(role);return QVariant();};
    int           columnCount(const QModelIndex &parent = QModelIndex())                           const override {Q_UNUSED(parent);return 1;};
    QVariant      data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

protected:
    QList<Route *> fetchData() const override;

private:
    Line *_line;
    LineDirection *_direction;
};

#endif // ROUTELISTMODEL_H
