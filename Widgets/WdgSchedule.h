#ifndef WDGSCHEDULE_H
#define WDGSCHEDULE_H

#include <QWidget>
#include <QUndoStack>
#include <QListWidget>
#include <QTableWidget>

#include "DataModels/ScheduleTableModel.h"
#include "DataModels/ScheduleTableFilterProxyModel.h"
#include "DataModels/LineDirectionListModel.h"
#include "DataModels/DayTypeListModel.h"
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
    void setCurrentLine(Line *);
    void setCurrenTrips(const QList<Trip *>);

private slots:
    void actionChangeDirection();
    void actionSetHourBreak(const int &);
    void actionOpenBusstopSchedule();

    bool checkMatchingWeekdays(WeekDays);
    void on_cmbDayTypes_activated(int index);

    void onSelectionChanged();
    void onCurrentCellChanged();
    void onColumnsInserted(QModelIndex parent, int first, int last);

    void showContextMenu();

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

    ScheduleTableModel *_model;
    ScheduleTableFilterProxyModel *_proxyModel;

    LineDirectionListModel *_directionsModel;

    DayTypeListModel *_dayTypesModel;

    QList<QAction *> _hourBreakActions;
    QAction *_actionCurrentBusstopName;
    QAction *_actionNew;
    QAction *_actionCopy;
    QAction *_actionOpenBusstopSchedule;
    QAction *_actionDelete;
    QAction *_actionShowOnlyImportantBusstops;

    Line *_currentLine;
    QList<Trip *> _currentTrips;

    bool _scheduleStartTimeChanging;
    QTime _ScheduleTmpOldStartTime;
};

#endif // WDGSCHEDULE_H
