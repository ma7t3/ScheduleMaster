#include "wdgtripeditor.h"
#include "ui_wdgtripeditor.h"

#include <QMessageBox>

#include "App/global.h"
#include "Commands/CmdSchedule.h"

#include "Dialogs/DlgCopyTrip.h"

WdgTripEditor::WdgTripEditor(QWidget *parent, ProjectData *projectData, QUndoStack *undoStack) :
    QWidget(parent),
    ui(new Ui::WdgTripEditor),
    projectData(projectData),
    undoStack(undoStack),
    _currentLine(nullptr),
    _currentDirection(nullptr),
    _currentDayType(global::getNewID()),
    _currentRoute(nullptr)
{
    ui->setupUi(this);

    QObject::connect(ui->pbNew, SIGNAL(clicked()), this, SLOT(actionNew()));
    QObject::connect(ui->pbCopy, SIGNAL(clicked()), this, SLOT(actionCopy()));
    QObject::connect(ui->pbDelete, SIGNAL(clicked()), this, SLOT(actionDelete()));

    QObject::connect(ui->cbTimeProfiles, SIGNAL(activated(int)), this, SLOT(actionChangeTimeProfile()));
    QObject::connect(ui->teDepartureTime, SIGNAL(timeChanged(QTime)), this, SLOT(actionChangeStartTime()));
    QObject::connect(ui->teDepartureTime, SIGNAL(editingFinished()), this, SLOT(saveStartTime()));

    QObject::connect(ui->daySelector, SIGNAL(weekDaysChanged()), this, SLOT(actionChangeDays()));
}

WdgTripEditor::~WdgTripEditor()
{
    delete ui;
}

void WdgTripEditor::setCurrentLine(Line *l, LineDirection *ld)
{
    if(l)
        _currentLine = l;
    else
        _currentLine = nullptr;

    if(ld)
        _currentDirection = ld;
    else
        _currentDirection = nullptr;

    refreshRoutes();
}

void WdgTripEditor::setCurrentTrips(QList<Trip *> trips) {
    _currentTrips = trips;
    changingTrips = true;
    refreshUI();
    changingTrips = false;
}

void WdgTripEditor::setCurrentDayType(DayType dt) {
    _currentDayType = dt;
}

void WdgTripEditor::actionNew() {
    if(!_currentLine || !_currentRoute)
        return;

    if(_currentRoute->timeProfileCount() == 0)
        return;

    TimeProfile *p = _currentRoute->timeProfileAt(0);

    QTime startTime = ui->teDepartureTime->time();
    if(!startTime.isValid())
        startTime.setHMS(0, 0, 0, 0);

    Trip *t = new Trip(global::getNewID(), _currentRoute, startTime, p, _currentDayType);
    undoStack->push(new cmdScheduleTripNew(_currentLine, t));
    _currentTrips = {t};
    emit tripsChanged(_currentTrips);
    refreshUI();
}

void WdgTripEditor::actionCopy() {
    if(_currentTrips.count() != 1)
        return;

    Trip *currentTrip = _currentTrips[0];

    DlgCopyTrip dlg(this, currentTrip->startTime());
    dlg.exec();
    if(dlg.result() != QDialog::Accepted)
        return;

    int count = dlg.copyCount();
    QTime interval = dlg.interval();

    QList<Trip *> trips;
    for(int i = 0; i < count; i++) {
        Trip *t = new Trip(global::getNewID(), currentTrip->route(), currentTrip->startTime(), currentTrip->timeProfile());
        t->overwrite(*currentTrip);
        t->setStartTime(QTime::fromMSecsSinceStartOfDay(currentTrip->startTime().msecsSinceStartOfDay() + ((i + 1) * interval.msecsSinceStartOfDay())));
        trips << t;
    }

    undoStack->push(new cmdScheduleTripsNew(_currentLine, trips));
    _currentTrips = {currentTrip};
    _currentTrips << trips;

    emit tripsChanged(_currentTrips);
}

void WdgTripEditor::actionDelete() {
    if(!_currentLine)
        return;

    QMessageBox::StandardButton msg = QMessageBox::warning(this, "Delete Trip", tr("Do you really want to delete these %n trips?", "", _currentTrips.count()), QMessageBox::Yes|QMessageBox::No);

    if(msg != QMessageBox::Yes)
        return;

    undoStack->push(new cmdScheduleTripsDelete(_currentLine, _currentTrips));
    _currentTrips.clear();
    emit tripsChanged(_currentTrips);
    refreshUI();
}

void WdgTripEditor::actionChangeRoute() {
    if(!_currentRoute || _currentTrips.empty())
        return;

    if(_currentRoute->profileCount() == 0)
        return;

    undoStack->push(new cmdScheduleTripsChangeRoute(_currentTrips, _currentRoute));
    emit tripsChanged(_currentTrips);
}

void WdgTripEditor::actionChangeTimeProfile() {
    if(_currentTrips.empty())
        return;

    QString profileName = ui->cbTimeProfiles->currentText();
    undoStack->push(new cmdScheduleTripsChangeTimeProfile(_currentTrips, profileName));
    emit tripsChanged(_currentTrips);
}

void WdgTripEditor::actionChangeStartTime() {
    if(_currentTrips.empty() || changingTrips)
        return;

    if(!ui->teDepartureTime->time().isValid())
        return;

    if(!startTimeChanging) {
        oldStartTimes.clear();
        for(int i = 0; i < _currentTrips.count(); i++)
            oldStartTimes << _currentTrips[i]->startTime();

        lastStartTime = oldStartTimes[0];
    }

    QTime time = ui->teDepartureTime->time();

    QTime timeDiff;
    bool later;
    if(time < lastStartTime) {
        later = false;
        timeDiff = QTime::fromMSecsSinceStartOfDay(lastStartTime.msecsSinceStartOfDay() - time.msecsSinceStartOfDay());
    } else {
        later = true;
        timeDiff = QTime::fromMSecsSinceStartOfDay(time.msecsSinceStartOfDay() - lastStartTime.msecsSinceStartOfDay());
    }

    for(int i = 0; i < _currentTrips.count(); i++) {
        Trip *t = _currentTrips[i];
        QTime currentTripStartTime;
        if(later)
            currentTripStartTime = QTime::fromMSecsSinceStartOfDay(t->startTime().msecsSinceStartOfDay() + timeDiff.msecsSinceStartOfDay());
        else
            currentTripStartTime = QTime::fromMSecsSinceStartOfDay(t->startTime().msecsSinceStartOfDay() - timeDiff.msecsSinceStartOfDay());
        t->setStartTime(currentTripStartTime);
    }

    startTimeChanging = true;

    lastStartTime = time;
    emit tripsChanged(_currentTrips);
}

void WdgTripEditor::actionChangeDays() {
    if(_currentTrips.empty())
        return;

    if(changingTrips)
        return;

    undoStack->push(new cmdScheduleTripsChangeDays(_currentTrips, ui->daySelector->weekDays()));
    emit tripsChanged(_currentTrips);
}

void WdgTripEditor::saveStartTime() {
    if(_currentTrips.empty() || changingTrips)
        return;

    QList<QTime> newStartTimes;

    for(int i = 0; i < _currentTrips.count(); i++)
        newStartTimes << _currentTrips[i]->startTime();

    startTimeChanging = false;
    undoStack->push(new cmdScheduleTripChangeStartTime(_currentTrips, oldStartTimes, newStartTimes));
}

void WdgTripEditor::refreshRoutes() {

    refreshingRoutes = true;

    ui->lwRoutes->clear();
    _routesListReference.clear();

    if(!_currentLine) {
        refreshingRoutes = false;
        return;
    }

    QList<Route *> routes = _currentLine->routes();
    routes = ProjectData::sortRoutes(routes);

    for(int i = 0; i < routes.count(); i++) {
        Route *r = routes[i];

        if(r->direction() != _currentDirection)
            continue;

        _routesListReference << r;

        QListWidgetItem *itm = new QListWidgetItem(r->name());

        if(r->profileCount() == 0) {
            itm->setHidden(true);
            itm->setToolTip(tr("You can't use this trip now, since it has no valid time profiles!"));
        }
        ui->lwRoutes->addItem(itm);

        if(r == _currentRoute)
            ui->lwRoutes->setCurrentRow(i);
    }

    refreshingRoutes = false;
}

void WdgTripEditor::refreshUI() {
    int tripCount = _currentTrips.count();
    QStringList selectedProfileNames;

    bool multipleRoutes = false;
    Route *route = nullptr;

    QStringList profileNames;

    QTime startTime, endTime;
    WeekDays weekDays;

    for(int i = 0; i < _currentTrips.count(); i++) {
        Trip *t = _currentTrips[i];

        if(route)
            if(route == t->route())
                continue;
            else
                multipleRoutes = true;
        else
            route = t->route();

        for(int j = 0; j < t->route()->timeProfileCount(); j++) {
            profileNames << t->route()->timeProfileAt(j)->name();
        }

        selectedProfileNames << t->timeProfile()->name();

        if(i == 0) {
            startTime = t->startTime();
            endTime = t->endTime();
            weekDays = *t->weekDays();
        }
    }

    profileNames.removeDuplicates();
    selectedProfileNames.removeDuplicates();

    QString currentProfileName = "";
    if(selectedProfileNames.count() == 1)
        currentProfileName = selectedProfileNames[0];

    int routeIndex = -1;
    for(int i = 0; i < _routesListReference.count(); i++)
        if(_routesListReference[i] == route)
            routeIndex = i;

    if(multipleRoutes && routeIndex != -1)
        ui->lwRoutes->setCurrentRow(-1);
    else {
        ui->lwRoutes->setCurrentRow(routeIndex);
        _currentRoute = route;
    }

    ui->cbTimeProfiles->clear();
    ui->cbTimeProfiles->addItems(profileNames);
    ui->cbTimeProfiles->setCurrentText(currentProfileName);
    if(currentProfileName == "")
        ui->cbTimeProfiles->setCurrentIndex(-1);

    if(tripCount != 0) {
        ui->teDepartureTime->setTime(startTime);
        ui->lArrivalTime->setText(endTime.toString("hh:mm:ss"));
    } else {
        ui->teDepartureTime->setTime(QTime(0, 0, 0, 0));
        ui->lArrivalTime->setText(QTime(0, 0, 0, 0).toString("hh:mm:ss"));
    }

    if(tripCount == 0) {
        ui->pbCopy->setEnabled(false);
        ui->pbDelete->setEnabled(false);
        ui->gbTiming->setEnabled(false);
        ui->gbDays->setEnabled(false);
        ui->daySelector->setWeekDays(WeekDays(0));
    } else if(tripCount == 1) {
        ui->pbCopy->setEnabled(true);
        ui->pbDelete->setEnabled(true);
        ui->gbTiming->setEnabled(true);
        ui->gbDays->setEnabled(true);

        ui->daySelector->setWeekDays(weekDays);
    } else {
        ui->pbCopy->setEnabled(false);
        ui->pbDelete->setEnabled(true);
        ui->gbTiming->setEnabled(true);
        ui->gbDays->setEnabled(false);
    }
}


void WdgTripEditor::on_lwRoutes_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous);

    if(refreshingRoutes || changingTrips)
        return;

    _currentRoute = nullptr;

    if(!current)
        return;

    int index = ui->lwRoutes->row(current);
    _currentRoute = _routesListReference[index];

    actionChangeRoute();
}

