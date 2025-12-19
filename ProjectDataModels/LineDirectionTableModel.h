#ifndef LINEDIRECTIONTABLEMODEL_H
#define LINEDIRECTIONTABLEMODEL_H

#include "ProjectDataModels/OrderedProjectDataRowModel.h"
#include "src/projectdata/Line.h"

#include <QObject>

class LineDirectionTableModel : public OrderedProjectDataRowModel<LineDirection> {
    Q_OBJECT

public:
    explicit LineDirectionTableModel(QObject *parent = nullptr);

    void setLine(Line *line);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    virtual PDIList<LineDirection> fetch() const override;

private:
    Line *_line;
};

#endif // LINEDIRECTIONTABLEMODEL_H
