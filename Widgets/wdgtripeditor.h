#ifndef WDGTRIPEDITOR_H
#define WDGTRIPEDITOR_H

#include <QWidget>
#include <QUndoStack>
#include <QListWidgetItem>

#include "DataModels/RouteListModel.h"
#include "ProjectData/projectdata.h"

namespace Ui {
class WdgTripEditor;
}

class WdgTripEditor : public QWidget
{
    Q_OBJECT

public:
    explicit WdgTripEditor(QWidget *parent);
    ~WdgTripEditor();

public slots:
    void setCurrentLine(Line *, LineDirection *);
    void setCurrentTrips(QList<Trip *>);
    void setCurrentDayType(DayType *);

    void actionNew();
    void actionCopy();
    void actionDelete();

private slots:
    void refreshUI();

    void actionChangeRoute();
    void actionChangeTimeProfile();
    void actionChangeStartTime();
    void actionChangeDays();

    void saveStartTime();

    void onCurrentRouteChanged(const QModelIndex &current, const QModelIndex &previous);

signals:
    void tripsChanged(const QList<Trip *> &);
    void refreshRequested();

private:
    Ui::WdgTripEditor *ui;

    ProjectData *projectData;
    RouteListModel *_model;

    Line *_currentLine;
    LineDirection *_currentDirection;
    QList<Trip *> _currentTrips;
    DayType *_currentDayType;

    Route *_currentRoute;

    bool _changingTrips = false;
    bool _startTimeChanging = false;
    QList<QTime> _oldStartTimes;
    QTime _lastStartTime;
};

#endif // WDGTRIPEDITOR_H
