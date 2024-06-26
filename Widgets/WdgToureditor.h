#ifndef WDGTOUREDITOR_H
#define WDGTOUREDITOR_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QUndoStack>

#include "ProjectData/projectdata.h"

namespace Ui {
class WdgTourEditor;
}

class WdgTourEditor : public QWidget
{
    Q_OBJECT

public:
    explicit WdgTourEditor(QWidget *parent = nullptr, ProjectData *projectData = nullptr, QUndoStack *undoStack = nullptr);
    ~WdgTourEditor();

public slots:
    void actionTourChangeProportionalRowHeight();
    void actionTourChangeShowBreaks();
    void actionTourChangeScale();
    void actionTourAddTrip();
    void actionTourRemoveTrip();
    void actionTourAddMoreTrips();
    void actionReorderTrips();
    void actionExport();
    
    void refresh();
    void refreshTourNextTrips();

    void setCurrentTour(Tour *t);

    Trip * currentTrip();
    Trip * currentNextTrip();

    void on_twTour_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

private slots:
    void on_twNextTrips_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

signals:
    void refreshRequested();

private:
    Ui::WdgTourEditor *ui;
    ProjectData *projectData;
    QUndoStack *undoStack;

    QList<Trip *> tourTableReference;
    QList<Trip *> nextTripsListReference;

    Tour *_currentTour = nullptr;
    Trip *_currentTrip = nullptr;
    Trip *_currentNextTrip = nullptr;

    //QString tourCurrentTrip;
};

#endif // WDGTOUREDITOR_H
