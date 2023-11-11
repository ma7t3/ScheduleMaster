#ifndef WDGSCHEDULE_H
#define WDGSCHEDULE_H

#include <QWidget>
#include <QUndoStack>
#include <QListWidget>
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
    /*void actionScheduleShowDaysMonFri();
    void actionScheduleShowDaysSat();
    void actionScheduleShowDaysSun();*/
    void actionChangeDirection();

    void actionScheduleTripNew();
    void actionScheduleTripDelete();
    void actionScheduleTripChangeRoute();
    void actionScheduleTripChangeProfile();
    void actionScheduleTripChangeStartTime();
    void actionScheduleTripChangeDays();
    void actionScheduleTripSwitchRepeat();
    void actionScheduleTripChangeRepeat();

    void setCurrentLine(Line *);
    void refreshDirections();
    void refreshDayTypes();

    void refreshRoutes();
    void refreshSchedule();

private slots:
    void refreshScheduleBusstopList(QList<Trip *>);
    void refreshScheduleAddTrip(Trip *);
    QPair<QString, QString> refreshScheduleGenerateInfo(WeekDays *);
    void refreshScheduleSelection();
    void refreshTripDetails();

    bool scheduleCheckMatchingWeekdays(WeekDays *);
    QList<Trip *> exectueTripRepetitions(QList<Trip *>);

    void on_twSchedule_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void on_twSchedule_itemPressed(QTableWidgetItem *item);

    void on_lwRoutes_itemClicked(QListWidgetItem *item);

    void on_teDepartureTime_editingFinished();

    void on_twSchedule_itemDoubleClicked(QTableWidgetItem *item);

    void on_cmbDayTypes_activated(int index);

signals:
    void busstopScheduleRequested(Busstop *, QList<Busstop *>, QList<Line *>, int);

private:
    Ui::WdgSchedule *ui;
    ProjectData *projectData;
    QUndoStack *undoStack;

    int headerRowCount = 6;

    Line *m_currentLine = nullptr;
    Trip *m_currentTrip = nullptr;
    Route *m_currentRoute = nullptr;
    QList<Trip *> scheduleTableTripsReference;
    QList<Busstop *> scheduleTableBusstopsReference;
    QList<Route *> routeTableReference;

    LineDirection *m_currentLineDirection = nullptr;
    QList<LineDirection *> lineDirectionsReference;

    DayType *m_currentDayType;
    QList<DayType *> dayTypesReference;

    //LineDirection *scheduleCurrentDirection = nullptr;

    bool scheduleStartTimeChanging = false;
    QTime ScheduleTmpOldStartTime;

    /*const int scheduleCurrentDaysMonFri = 1;
    const int scheduleCurrentDaysSat = 2;
    const int scheduleCurrentDaysSun = 3;
    int scheduleCurrentDaysShow = scheduleCurrentDaysMonFri;*/

    void scheduleSaveProfile();
    void scheduleSaveStartTime();
};

#endif // WDGSCHEDULE_H
