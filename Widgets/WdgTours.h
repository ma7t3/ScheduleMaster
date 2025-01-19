#ifndef WDGTOURS_H
#define WDGTOURS_H

#include <QWidget>
#include <QUndoStack>
#include <QTableWidget>

#include "ProjectData/projectdata.h"
#include "DataModels/TourTableModel.h"

namespace Ui {
class WdgTours;
}

class WdgTours : public QWidget
{
    Q_OBJECT

public:
    explicit WdgTours(QWidget *parent);
    ~WdgTours();

    QList<QAction *> actions();

public slots:
    void refreshUI();
    Tour *currentTour() const;
    void setCurrentTour(Tour *);

private slots:
    void actionNew();
    void actionEdit();
    void actionDuplicate();
    void actionDelete();
    void actionExport();

    void onSelectionChanged();
    void onRowsInserted(QModelIndex parent, int first, int last);

signals:
    void currentTourChanged(Tour *);
    void refreshRequested();

private:
    Ui::WdgTours *ui;

    ProjectData *projectData;
    TourTableModel *_model;
    QSortFilterProxyModel *_proxyModel;
    Tour *_currentTour = nullptr;

    QAction *_actionNew;
    QAction *_actionEdit;
    QAction *_actionDuplicate;
    QAction *_actionDelete;
    QAction *_actionExport;
};

#endif // WDGTOURS_H
