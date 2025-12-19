#ifndef ROUTETABLEMODEL_H
#define ROUTETABLEMODEL_H

#include "ProjectDataModels/UnorderedProjectDataRowModel.h"
#include "src/projectdata/Line.h"

class RouteTableModel : public UnorderedProjectDataRowModel<Route> {
    Q_OBJECT
public:
    explicit RouteTableModel(QObject *parent = nullptr);

    Line *line() const;
    void setLine(Line *line);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    virtual PDISet<Route> fetch() const override;
private:
    Line *_line;
};

#endif // ROUTETABLEMODEL_H
