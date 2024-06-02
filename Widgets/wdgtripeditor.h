#ifndef WDGTRIPEDITOR_H
#define WDGTRIPEDITOR_H

#include <QWidget>
#include <QUndoStack>
#include <QListWidgetItem>

#include "ProjectData/projectdata.h"

namespace Ui {
class WdgTripEditor;
}

class WdgTripEditor : public QWidget
{
    Q_OBJECT

public:
    explicit WdgTripEditor(QWidget *parent = nullptr, ProjectData *projectData = nullptr, QUndoStack *undoStack = nullptr);
    ~WdgTripEditor();

public slots:
    void setCurrentLine(Line *, LineDirection *);
    void setCurrentTrips(QList<Trip *>);
    void setCurrentDayType(DayType *);

    void actionNew();
    void actionCopy();
    void actionDelete();

    void actionChangeRoute();

    void actionChangeTimeProfile();
    void actionChangeStartTime();

    void actionChangeDays();

    void saveStartTime();

    void refreshRoutes();

    void refreshUI();

signals:
    void tripsChanged(const QList<Trip *> &);
    void refreshRequested();

private slots:
    void on_lwRoutes_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::WdgTripEditor *ui;

    ProjectData *projectData;
    QUndoStack *undoStack;

    Line *_currentLine;
    LineDirection *_currentDirection;
    QList<Trip *> _currentTrips;
    DayType *_currentDayType;

    QList<Route *> _routesListReference;
    Route *_currentRoute;

    bool refreshingRoutes = false;
    bool changingTrips = false;
    bool startTimeChanging = false;
    QList<QTime> oldStartTimes;
    QTime lastStartTime;
};

#endif // WDGTRIPEDITOR_H
