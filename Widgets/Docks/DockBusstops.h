#ifndef DOCKBUSSTOPS_H
#define DOCKBUSSTOPS_H

#include <QWidget>
#include <QSortFilterProxyModel>

#include "DockAbstract.h"
#include "ProjectDataModels/BusstopTableModel.h"
#include "Widgets/WdgTableColumnVisibilitySelector.h"

namespace Ui {
class DockBusstops;
}

class DockBusstops : public DockAbstract {
    Q_OBJECT

public:
    explicit DockBusstops(QWidget *parent = nullptr);
    ~DockBusstops();

    Busstop *currentBusstop() const;
    PDISet<Busstop> selectedBusstops() const;

protected slots:
    void onBusstopNew();
    void onBusstopEdit();
    void onBusstopDelete();

    void onSelectionChanged();
    void onRowsAdded(const QList<QPersistentModelIndex> &indexes);

signals:
    void currentBusstopChanged(Busstop *);
    void selectedBusstopsChaned(PDISet<Busstop>);

private:
    Ui::DockBusstops *ui;
    QAction *_newAction, *_editAction, *_deleteAction, *_searchAction;
    BusstopTableModel *_model;
    QSortFilterProxyModel *_proxyModel;
    BusstopTableModelDelegate *_delegate;

    WdgTableColumnVisibilitySelector *_columnVisibilitySelector;

    ProjectData *_projectData;
    Busstop *_currentBusstop;
};

#endif // DOCKBUSSTOPS_H
