#include "wdgtripeditor.h"
#include "ui_wdgtripeditor.h"

#include <QMessageBox>

#include "Mainwindow.h"
#include "Commands/CmdSchedule.h"
#include "Dialogs/DlgCopyTrip.h"
#include "localconfig.h"

WdgTripEditor::WdgTripEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgTripEditor),
    projectData(dynamic_cast<MainWindow *>(parent)->projectData()),
    _model(new RouteListModel(this)),
    _currentLine(nullptr),
    _currentDirection(nullptr),
    _currentDayType(nullptr),
    _currentRoute(nullptr) {

    ui->setupUi(this);

    ui->teDepartureTime->setDisplayFormat(LocalConfig::timeFormatString(true, false));
    ui->lwRoutes->setModel(_model);

    connect(ui->pbNew,                      &QPushButton::clicked,                this, &WdgTripEditor::actionNew);
    connect(ui->pbCopy,                     &QPushButton::clicked,                this, &WdgTripEditor::actionCopy);
    connect(ui->pbDelete,                   &QPushButton::clicked,                this, &WdgTripEditor::actionDelete);

    connect(ui->cbTimeProfiles,             &QComboBox::activated,                this, &WdgTripEditor::actionChangeTimeProfile);
    connect(ui->teDepartureTime,            &QTimeEdit::timeChanged,              this, &WdgTripEditor::actionChangeStartTime);
    connect(ui->teDepartureTime,            &QTimeEdit::editingFinished,          this, &WdgTripEditor::saveStartTime);

    connect(ui->daySelector,                &WdgDaySelector::weekDaysChanged,     this, &WdgTripEditor::actionChangeDays);

    connect(ui->lwRoutes->selectionModel(), &QItemSelectionModel::currentChanged, this, &WdgTripEditor::onCurrentRouteChanged);
}

WdgTripEditor::~WdgTripEditor()
{
    delete ui;
}

void WdgTripEditor::setCurrentLine(Line *l, LineDirection *ld) {
    if(!l || !ld) {
        _currentLine = nullptr;
        _currentDirection = nullptr;
    } else {
        _currentLine = l;
        _currentDirection = ld;
    }

    _model->setDirection(ld);
}

void WdgTripEditor::setCurrentTrips(QList<Trip *> trips) {
    _currentTrips = trips;
    _changingTrips = true;
    refreshUI();
    _changingTrips = false;
}

void WdgTripEditor::setCurrentDayType(DayType *dt) {
    _currentDayType = dt;
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
            weekDays = t->weekDays();
        }
    }

    profileNames.removeDuplicates();
    selectedProfileNames.removeDuplicates();

    QString currentProfileName = "";
    if(selectedProfileNames.count() == 1)
        currentProfileName = selectedProfileNames[0];

    int routeIndex = -1;
    for(int i = 0; i < _model->itemCount(); i++)
        if(_model->itemAt(i) == route)
            routeIndex = i;

    if(multipleRoutes && routeIndex != -1)
        ui->lwRoutes->setCurrentIndex(QModelIndex());
    else {
        ui->lwRoutes->setCurrentIndex(_model->index(routeIndex, 0));
        _currentRoute = route;
    }

    ui->cbTimeProfiles->clear();
    ui->cbTimeProfiles->addItems(profileNames);
    ui->cbTimeProfiles->setCurrentText(currentProfileName);
    if(currentProfileName == "")
        ui->cbTimeProfiles->setCurrentIndex(-1);

    if(tripCount != 0) {
        ui->teDepartureTime->setTime(startTime);
        ui->lArrivalTime->setText(endTime.toString(LocalConfig::timeFormatString(true, false)));
    } else {
        ui->teDepartureTime->setTime(QTime(0, 0, 0, 0));
        ui->lArrivalTime->setText(QTime(0, 0, 0, 0).toString(LocalConfig::timeFormatString(true, false)));
    }

    if(tripCount == 0) {
        ui->pbCopy->setEnabled(false);
        ui->pbDelete->setEnabled(false);
        ui->gbTiming->setEnabled(false);
        ui->gbDays->setEnabled(false);
        ui->daySelector->setWeekDays(WeekDays());
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

void WdgTripEditor::actionNew() {
    if(!_currentLine || !_currentRoute)
        return;

    if(_currentRoute->timeProfileCount() == 0)
        return;

    if(!_currentDayType)
        return;

    TimeProfile *p = _currentRoute->timeProfileAt(0);

    QTime startTime = ui->teDepartureTime->time();
    if(!startTime.isValid())
        startTime.setHMS(0, 0, 0, 0);

    Trip *t = _currentLine->newTrip();
    t->setRoute(_currentRoute);
    t->setStartTime(startTime);
    t->setTimeProfile(p);
    t->setWeekDays(*_currentDayType);
    projectData->undoStack()->push(new CmdScheduleTripNew(_currentLine, t));
    _currentTrips = {t};
    emit tripsChanged(_currentTrips);
    emit refreshRequested();
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
        Trip *t = _currentLine->newTrip();
        t->setRoute(currentTrip->route());
        t->setStartTime(QTime::fromMSecsSinceStartOfDay(currentTrip->startTime().msecsSinceStartOfDay() + ((i + 1) * interval.msecsSinceStartOfDay())));
        t->setWeekDays(currentTrip->weekDays());
        t->setTimeProfile(currentTrip->timeProfile());
        trips << t;
    }

    projectData->undoStack()->push(new CmdScheduleTripsNew(_currentLine, trips));
    _currentTrips = {currentTrip};
    _currentTrips << trips;

    emit tripsChanged(_currentTrips);
    emit refreshRequested();
}

void WdgTripEditor::actionDelete() {
    if(!_currentLine)
        return;

    QMessageBox::StandardButton msg = QMessageBox::warning(this, "Delete Trip", tr("Do you really want to delete these %n trips?", "", _currentTrips.count()), QMessageBox::Yes|QMessageBox::No);

    if(msg != QMessageBox::Yes)
        return;

    projectData->undoStack()->push(new CmdScheduleTripsDelete(_currentLine, _currentTrips));
    _currentTrips.clear();
    emit tripsChanged(_currentTrips);
    emit refreshRequested();
    refreshUI();
}

void WdgTripEditor::actionChangeRoute() {
    if(!_currentRoute || _currentTrips.empty())
        return;

    if(_currentRoute->timeProfileCount() == 0)
        return;

    projectData->undoStack()->push(new CmdScheduleTripsChangeRoute(_currentTrips, _currentRoute));
    emit tripsChanged(_currentTrips);
    emit refreshRequested();
}

void WdgTripEditor::actionChangeTimeProfile() {
    if(_currentTrips.empty())
        return;

    QString profileName = ui->cbTimeProfiles->currentText();
    projectData->undoStack()->push(new CmdScheduleTripsChangeTimeProfile(_currentTrips, profileName));
    emit tripsChanged(_currentTrips);
    emit refreshRequested();
}

void WdgTripEditor::actionChangeStartTime() {
    if(_currentTrips.empty() || _changingTrips)
        return;

    if(!ui->teDepartureTime->time().isValid())
        return;

    if(!_startTimeChanging) {
        _oldStartTimes.clear();
        for(int i = 0; i < _currentTrips.count(); i++)
            _oldStartTimes << _currentTrips[i]->startTime();

        _lastStartTime = _oldStartTimes[0];
    }

    QTime time = ui->teDepartureTime->time();

    QTime timeDiff;
    bool later;
    if(time < _lastStartTime) {
        later = false;
        timeDiff = QTime::fromMSecsSinceStartOfDay(_lastStartTime.msecsSinceStartOfDay() - time.msecsSinceStartOfDay());
    } else {
        later = true;
        timeDiff = QTime::fromMSecsSinceStartOfDay(time.msecsSinceStartOfDay() - _lastStartTime.msecsSinceStartOfDay());
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

    _startTimeChanging = true;

    _lastStartTime = time;
    emit tripsChanged(_currentTrips);
    emit refreshRequested();
}

void WdgTripEditor::actionChangeDays() {
    if(_currentTrips.empty())
        return;

    if(_changingTrips)
        return;

    projectData->undoStack()->push(new CmdScheduleTripsChangeDays(_currentTrips, ui->daySelector->weekDays()));
    emit tripsChanged(_currentTrips);
    emit refreshRequested();
}

void WdgTripEditor::saveStartTime() {
    if(_currentTrips.empty() || _changingTrips)
        return;

    QList<QTime> newStartTimes;

    for(int i = 0; i < _currentTrips.count(); i++)
        newStartTimes << _currentTrips[i]->startTime();

    _startTimeChanging = false;
    projectData->undoStack()->push(new CmdScheduleTripChangeStartTime(_currentTrips, _oldStartTimes, newStartTimes));
}

void WdgTripEditor::onCurrentRouteChanged(const QModelIndex &current, const QModelIndex &previous) {
    Q_UNUSED(previous);

    if(_changingTrips)
        return;

    _currentRoute = nullptr;

    if(!current.isValid())
        _currentRoute = nullptr;
    else
        _currentRoute = _model->itemAt(current.row());

    actionChangeRoute();
}

