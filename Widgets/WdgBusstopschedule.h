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
    void setDays(int);

    void setAll(Busstop *, QList<Route *> routes = {}, int days = 1);

private slots:
    void on_rbMonFri_clicked();

    void on_rbSat_clicked();

    void on_rbSun_clicked();


private slots:
    void refreshSchedule();

private:
    Ui::WdgBusstopSchedule *ui;

    void showEvent(QShowEvent *e) override;
    WeekDays getShiftedWeekDays(Trip *);
    bool checkMatchingWeekDays(WeekDays);

    bool refreshing;

    ProjectData *projectData;
    Busstop *busstop;
    QList<Busstop *> preselectedDirections;
    QList<Line *> preselectedLines;
};

#endif // BUSSTOPSCHEDULEVIEWER_H
