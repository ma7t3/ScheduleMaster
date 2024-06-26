#ifndef BUSSTOPSCHEDULEVIEWER_H
#define BUSSTOPSCHEDULEVIEWER_H

#include <QDialog>
#include "ProjectData\projectdata.h"

#include <QTreeWidgetItem>

namespace Ui {
class WdgBusstopSchedule;
}

class WdgBusstopSchedule : public QWidget
{
    Q_OBJECT

public:
    explicit WdgBusstopSchedule(QWidget *parent = nullptr, ProjectData* = nullptr);
    ~WdgBusstopSchedule();

public slots:
    void setBusstop(Busstop *);
    void setRoutes(QList<Route *>);
    void setAll(Busstop *, QList<Route *> routes = {}, DayType * = nullptr);

private slots:
    void refreshSchedule();

    void on_cmbDayTypes_activated(int index);

private:
    Ui::WdgBusstopSchedule *ui;

    void showEvent(QShowEvent *e) override;
    WeekDays getShiftedWeekDays(Trip *);
    bool checkMatchingWeekDays(WeekDays);

    bool refreshing;
    DayType *currentDayType;

    ProjectData *projectData;
    Busstop *busstop;
    QList<Busstop *> preselectedDirections;
    QList<Line *> preselectedLines;
};

#endif // BUSSTOPSCHEDULEVIEWER_H
