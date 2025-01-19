#ifndef TOURTABLEMODEL_H
#define TOURTABLEMODEL_H

#include "DataModels/UnorderedProjectDataRowModel.h"
#include "localconfig.h"

#include <QFont>
#include <QPixmap>
#include <QPainter>

class TourTableModel : public UnorderedProjectDataRowModel<Tour> {
    Q_OBJECT

public:
    explicit TourTableModel(QObject *parent);
    void     setProjectData(ProjectData *);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int      columnCount(const QModelIndex &parent = QModelIndex())                           const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;

protected:
    QList<Tour *> fetchData() const override;

    QColor timeColor(Tour *) const;
    QColor durationColor(Tour *, bool foreground = false) const;
    QPixmap previewPixmap(Tour *o) const;


private:
    ProjectData *projectData;
};

#endif // TOURTABLEMODEL_H
