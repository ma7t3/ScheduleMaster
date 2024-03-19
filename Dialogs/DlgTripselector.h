#ifndef TRIPSELECTOR_H
#define TRIPSELECTOR_H

#include <QDialog>

#include "ProjectData\projectdata.h"
#include "ProjectData\busstop.h"
#include <QTreeWidget>

namespace Ui {
class TripSelector;
}

class TripSelector : public QDialog
{
    Q_OBJECT

public:
    explicit TripSelector(QWidget *parent = nullptr, ProjectData *p = new ProjectData, Trip *t = nullptr, WeekDays d = WeekDays());
    ~TripSelector();

    Trip * getTrip();

private slots:
    void on_twLines_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_twTrips_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_cmbDirections_activated(int index);

private:
    void loadLines();
    void loadTrips();

    Ui::TripSelector *ui;
    ProjectData *projectData;
    Busstop *startBusstop;
    QTime startTime;
    Trip *previousTrip;
    WeekDays weekDays;

    LineDirection *currentDirection = nullptr;
    QList<LineDirection *> directionsReference;
};

#endif // TRIPSELECTOR_H
