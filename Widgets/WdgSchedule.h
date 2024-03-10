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
    void actionChangeDirection();

    void setCurrentLine(Line *);
    void refreshDirections();
    void refreshDayTypes();

    //void refreshRoutes();
    void refreshSchedule();
    void refreshSchedule(QList<Trip *>);

private slots:
    void refreshScheduleBusstopList(QList<Trip *>);
    void refreshScheduleAddTrip(Trip *);
    QPair<QString, QString> refreshScheduleGenerateInfo(WeekDays *);

    bool scheduleCheckMatchingWeekdays(WeekDays *);

    void on_twSchedule_itemDoubleClicked(QTableWidgetItem *item);

    void on_cmbDayTypes_activated(int index);

    void on_twSchedule_itemSelectionChanged();

signals:
    void currentLineChanged(Line *, LineDirection *);
    void currentDirectionChanged(LineDirection *);
    void currentTripsChanged(QList<Trip *>);
    void currentDayTypeChanged(DayType);
    void busstopScheduleRequested(Busstop *, QList<Route *>, int);
    void tourRequested(Tour *);

private:
    Ui::WdgSchedule *ui;
    ProjectData *projectData;
    QUndoStack *undoStack;

    int headerRowCount = 6;

    Line *_currentLine = nullptr;
    QList<Trip *> _currentTrips;
    //Trip *_currentTrip;
    Route *_currentRoute = nullptr;
    QList<Trip *> scheduleTableTripsReference;
    QList<Busstop *> scheduleTableBusstopsReference;
    QList<Route *> routeTableReference;

    LineDirection *_currentLineDirection = nullptr;
    QList<LineDirection *> lineDirectionsReference;

    DayType *_currentDayType;
    QList<DayType *> dayTypesReference;

    bool scheduleStartTimeChanging = false;
    QTime ScheduleTmpOldStartTime;

    bool refreshingSchedule = false;

    void scheduleSaveProfile();
    void scheduleSaveStartTime();
};

#endif // WDGSCHEDULE_H
