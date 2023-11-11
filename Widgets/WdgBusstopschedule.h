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

    //void setProjectData(ProjectData *);

public slots:
    void setBusstop(Busstop *);
    void setDirections(QList<Busstop *>);
    void setLines(QList<Line *>);
    void setDays(int);

    void setAll(Busstop *, QList<Busstop *> directions = {}, QList<Line *> lines = {}, int days = 1);

private slots:
    void on_twDirections_itemChanged();

    void on_twLines_itemChanged();

    void on_rbMonFri_clicked();

    void on_rbSat_clicked();

    void on_rbSun_clicked();



private:
    Ui::WdgBusstopSchedule *ui;

    void showEvent(QShowEvent *e) override;

    void refreshLinesAndDirections();
    void refreshSchedule();
    WeekDays getShiftedWeekDays(Trip *);
    bool checkMatchingWeekDays(WeekDays);

    bool refreshing;

    ProjectData *projectData;
    Busstop *busstop;
    QList<Busstop *> preselectedDirections;
    QList<Line *> preselectedLines;
};

#endif // BUSSTOPSCHEDULEVIEWER_H
