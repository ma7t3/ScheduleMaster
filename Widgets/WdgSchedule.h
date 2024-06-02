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

class WdgSchedule : public QWidget {
    Q_OBJECT

public:
    explicit WdgSchedule(QWidget *parent);
    ~WdgSchedule();

    QList<QAction *> actions();
    QList<QAction *> hourBreakActions();

public slots:
    void refreshUI();
    void refreshHourBreak();
    void refreshSchedule();
    void refreshDirections();
    void refreshDayTypes();
    void setCurrentLine(Line *);
    void setCurrenTrips(const QList<Trip *>);

private slots:
    void actionChangeDirection();
    void actionSetHourBreak(const int &);
    void actionOpenBusstopSchedule();

    void refreshBusstopList(QList<Trip *>);
    void refreshAddTrip(Trip *);

    bool checkMatchingWeekdays(WeekDays);
    void on_cmbDayTypes_activated(int index);
    void refreshReferences();

signals:
    void currentLineChanged(Line *, LineDirection *);
    void currentDirectionChanged(LineDirection *);
    void currentTripsChanged(QList<Trip *>);
    void currentDayTypeChanged(DayType *);
    void busstopScheduleRequested(Busstop *, QList<Route *>, DayType *);
    void tourRequested(Tour *);
    void refreshRequested();

private:
    Ui::WdgSchedule *ui;
    ProjectData *projectData;

    static const inline int headerRowCount = 7;

    QList<QAction *> _hourBreakActions;

    QAction *_actionNew;
    QAction *_actionCopy;
    QAction *_actionOpenBusstopSchedule;
    QAction *_actionDelete;
    QAction *_actionShowOnlyImportantBusstops;

    Line *_currentLine;
    QList<Trip *> _currentTrips;
    QList<Trip *> _scheduleTableTripsReference;
    QList<Busstop *> _scheduleTableBusstopsReference;
    QList<Route *> _routeTableReference;

    LineDirection *_currentLineDirection;
    QList<LineDirection *> _lineDirectionsReference;

    DayType *_currentDayType;
    QList<DayType *> _dayTypesReference;

    bool _scheduleStartTimeChanging;
    QTime _ScheduleTmpOldStartTime;
};

#endif // WDGSCHEDULE_H
