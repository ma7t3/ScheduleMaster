#ifndef WDGSCHEDULE_H
#define WDGSCHEDULE_H

#include <QWidget>
#include <QUndoStack>
#include <QTreeWidget>
#include <QTableWidget>

#include "ProjectData/projectdata.h"

namespace Ui {
class WdgSchedule;
}

class WdgSchedule : public QWidget
{
    Q_OBJECT

public:
    explicit WdgSchedule(QWidget *parent = nullptr, ProjectData *projectData = nullptr, QUndoStack *undoStack = nullptr);
    ~WdgSchedule();

public slots:
    void actionScheduleShowTripForward();
    void actionScheduleShowTripReverse();
    void actionScheduleChangeShowOnlyImportantBusstops();
    void actionScheduleSearchBusstop();
    void actionScheduleShowDaysMonFri();
    void actionScheduleShowDaysSat();
    void actionScheduleShowDaysSun();

    void actionScheduleTripNew();
    void actionScheduleTripDelete();
    void actionScheduleTripChangeRoute();
    void actionScheduleTripChangeProfile();
    void actionScheduleTripChangeStartTime();
    void actionScheduleTripChangeDays();
    void actionScheduleTripSwitchRepeat();
    void actionScheduleTripChangeRepeat();

    void refreshSchedule();
    void refreshSchedule(Line *);
    void refreshScheduleRoutes();
    void refreshScheduleBusstopList(QList<Trip *>);
    void refreshScheduleAddTrip(Trip *);
    QPair<QString, QString> refreshScheduleGenerateInfo(WeekDays *);
    void refreshScheduleSelection();
    void refreshTripDetails();

    //Line * getScheduleSelectedLine();
    Route * getScheduleSelectedRoute();
    Trip * getScheduleSelectedTrip();

    void setCurrentLine(Line *);

    //void on_twScheduleLines_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_twSchedule_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void on_twSchedule_cellDoubleClicked(int row, int column);
    void on_teScheduleDep_editingFinished();



private:
    Ui::WdgSchedule *ui;
    ProjectData *projectData;
    QUndoStack *undoStack;

    Line *currentLine;
    QString scheduleCurrentTripId;
    QString scheduleCurrentLineId;
    bool scheduleCurrentDirection = true;
    int scheduleCurrentDaysShow = 1;

    bool scheduleStartTimeChanging = false;
    QTime ScheduleTmpOldStartTime;

    const int scheduleCurrentDaysMoFr = 1;
    const int scheduleCurrentDaysSa = 2;
    const int scheduleCurrentDaysSo = 3;

    bool scheduleCheckMatchingWeekdays(WeekDays *);
    QList<Trip *> scheduleExectureTripRepetitions(QList<Trip *>);
    void scheduleSaveProfile();
    void scheduleSaveStartTime();
};

#endif // WDGSCHEDULE_H
