#include "wdgschedule.h"
#include "ui_wdgschedule.h"

#include <QMessageBox>
#include <QScrollBar>

#include "global.h"
#include "ProjectData/projectdata.h"
#include "Commands/schedule.h"

WdgSchedule::WdgSchedule(QWidget *parent, ProjectData *projectData, QUndoStack *undoStack) :
    QWidget(parent),
    ui(new Ui::WdgSchedule),
    projectData(projectData),
    undoStack(undoStack),
    currentLine(nullptr)
{
    ui->setupUi(this);

    QObject::connect(ui->pbScheduleForwardTrip, SIGNAL(clicked()), this, SLOT(actionScheduleShowTripForward()));
    QObject::connect(ui->pbScheduleReverseTrip, SIGNAL(clicked()), this, SLOT(actionScheduleShowTripReverse()));
    QObject::connect(ui->cbScheduleOnlyImportantBusstops, SIGNAL(stateChanged(int)), this, SLOT(actionScheduleChangeShowOnlyImportantBusstops()));
    QObject::connect(ui->leScheduleSearchBusstop, SIGNAL(textChanged(QString)), this, SLOT(actionScheduleSearchBusstop()));
    QObject::connect(ui->pbScheduleDaysMonFri, SIGNAL(clicked()), this, SLOT(actionScheduleShowDaysMonFri()));
    QObject::connect(ui->pbScheduleDaysSat, SIGNAL(clicked()), this, SLOT(actionScheduleShowDaysSat()));
    QObject::connect(ui->pbScheduleDaysSun, SIGNAL(clicked()), this, SLOT(actionScheduleShowDaysSun()));

    QObject::connect(ui->pbScheduleNew, SIGNAL(clicked()), this, SLOT(actionScheduleTripNew()));
    QObject::connect(ui->pbScheduleDelete, SIGNAL(clicked()), this, SLOT(actionScheduleTripDelete()));
    QObject::connect(ui->twScheduleRoutes, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(actionScheduleTripChangeRoute()));
    QObject::connect(ui->cbScheduleProfiles, SIGNAL(activated(int)), this, SLOT(actionScheduleTripChangeProfile()));
    /*QObject::connect(ui->teScheduleDep, SIGNAL(userTimeChanged(QTime)), this, SLOT(actionScheduleTripChangeStartTime()));*/

    QObject::connect(ui->cbScheduleDayMon, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbScheduleDayTue, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbScheduleDayWed, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbScheduleDayThu, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbScheduleDayFri, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbScheduleDaySat, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbScheduleDaySun, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbScheduleDayHol, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbScheduleDaySco, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbScheduleDayNsco, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));

    QObject::connect(ui->gbScheduleRepeat, SIGNAL(clicked()), this, SLOT(actionScheduleTripSwitchRepeat()));
    QObject::connect(ui->pbScheduleTripDetailsRepeatApply, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeRepeat()));

    ui->twScheduleRoutes->setColumnHidden(0, true);
    ui->twScheduleRoutes->setHeaderHidden(true);

    ui->twSchedule->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->twSchedule->horizontalHeader()->setVisible(false);
    ui->twSchedule->setColumnHidden(0, true);
    //ui->twSchedule->setRowHidden(0, true);
    //ui->twSchedule->setRowHidden(1, true);
}

WdgSchedule::~WdgSchedule()
{
    delete ui;
}

void WdgSchedule::actionScheduleShowTripForward() {
    scheduleCurrentDirection = true;
    ui->pbScheduleForwardTrip->setDefault(true);
    ui->pbScheduleReverseTrip->setDefault(false);
    refreshSchedule();
    refreshScheduleRoutes();
}

void WdgSchedule::actionScheduleShowTripReverse() {
    scheduleCurrentDirection = false;
    ui->pbScheduleForwardTrip->setDefault(false);
    ui->pbScheduleReverseTrip->setDefault(true);
    refreshSchedule();
    refreshScheduleRoutes();
}

void WdgSchedule::actionScheduleChangeShowOnlyImportantBusstops() {
    refreshSchedule();
}

void WdgSchedule::actionScheduleSearchBusstop() {
    refreshSchedule();
}

void WdgSchedule::actionScheduleShowDaysMonFri() {
    scheduleCurrentDaysShow = 1;
    ui->pbScheduleDaysMonFri->setDefault(true);
    ui->pbScheduleDaysSat->setDefault(false);
    ui->pbScheduleDaysSun->setDefault(false);
    refreshSchedule();
}

void WdgSchedule::actionScheduleShowDaysSat() {
    scheduleCurrentDaysShow = 2;
    ui->pbScheduleDaysMonFri->setDefault(false);
    ui->pbScheduleDaysSat->setDefault(true);
    ui->pbScheduleDaysSun->setDefault(false);
    refreshSchedule();
}

void WdgSchedule::actionScheduleShowDaysSun() {
    scheduleCurrentDaysShow = 3;
    ui->pbScheduleDaysMonFri->setDefault(false);
    ui->pbScheduleDaysSat->setDefault(false);
    ui->pbScheduleDaysSun->setDefault(true);
    refreshSchedule();
}

void WdgSchedule::actionScheduleTripNew() {
    Line *l = currentLine;
    Route *r = getScheduleSelectedRoute();

    if(!l || !r)
        return;
    
    TimeProfile *p = r->timeProfileAt(0);
    if(!p)
        return;

    QTime time = ui->teScheduleDep->time();
    if(!time.isValid())
        time.setHMS(0, 0, 0, 0);

    Trip *t = new Trip(global::getNewID(), r, time, p);
    
    WeekDays *d = t->weekDays();
    t->setRepeat(false);

    if(scheduleCurrentDaysShow == 1)
        d->setCode(WeekDays::MonFri);
    else if(scheduleCurrentDaysShow == 2)
        d->setCode(WeekDays::Sat);
    else if(scheduleCurrentDaysShow == 3)
        d->setCode(WeekDays::Sun);
    undoStack->push(new cmdScheduleTripNew(l, t, scheduleCurrentDirection));

    scheduleCurrentTripId = t->id();
    refreshSchedule();
}

void WdgSchedule::actionScheduleTripDelete() {
    Line *l = currentLine;
    if(!l)
        return;

    Trip *t = getScheduleSelectedTrip();
    if(!t)
        return;
    
    QString tripName    = t->route()->name();
    QString startTime   = t->startTime().toString("hh:mm");
    QString endTime     = t->endTime().toString("hh:mm");
    QString firstStop   = t->route()->busstopAt(0)->name();
    QString lastStop    = t->route()->busstopAt(t->route()->busstopCount() - 1)->name();
    QString repeat      = t->hasRepeat() ? "Yes" : "No";

    QString msgStr = QString("<p><b>Do you really want do delete this trip?</b></p><table><tr><td><b>Name:</b></td><td colspan=\"2\">%1</td></tr><tr><td><b>Start:</b></td><td>%2</td><td>%3</td></tr><tr><td><b>End:</b></td><td>%4</td><td>%5</td></tr><tr><td><b>Repeat:</b></td><td colspan=\"2\">%6</td></tr></table>").arg(tripName, startTime, firstStop, endTime, lastStop, repeat);

    QMessageBox::StandardButton msg = QMessageBox::warning(this, "deleteTrip", msgStr, QMessageBox::Yes|QMessageBox::No);

    if(msg != QMessageBox::Yes)
        return;

    undoStack->push(new cmdScheduleTripDelete(l, t));
    refreshSchedule();
}

void WdgSchedule::actionScheduleTripChangeRoute() {
    if(!ui->twScheduleRoutes->currentItem())
        return;

    Trip *t = getScheduleSelectedTrip();
    if(!t)
        return;

    Route *r = getScheduleSelectedRoute();
    if(!r)
        return;
    
    if(r->profileCount() == 0)
        return;

    int targetProfileIndex = ui->cbScheduleProfiles->currentIndex();
    targetProfileIndex = r->profileCount() > targetProfileIndex ? targetProfileIndex : 0;
    TimeProfile *newProfile = r->timeProfileAt(targetProfileIndex);

    undoStack->push(new cmdScheduleTripChangeRoute(t, r, newProfile));

    refreshSchedule();
    refreshTripDetails();
}

void WdgSchedule::actionScheduleTripChangeProfile() {
    int index = ui->cbScheduleProfiles->currentIndex();

    Trip *t = getScheduleSelectedTrip();
    if(!t)
        return;
    
    Route *r = t->route();
    TimeProfile *p = r->timeProfileAt(index);
    if(!p)
        return;

    undoStack->push(new cmdScheduleTripChangeTimeProfile(t, p));
    refreshSchedule();
    refreshTripDetails();
}

void WdgSchedule::actionScheduleTripChangeStartTime() {
    if(ui->twSchedule->hasFocus())
        return;

    Trip *t = getScheduleSelectedTrip();
    if(!t)
        return;

    if(!ui->teScheduleDep->time().isValid())
        return;

    if(!scheduleStartTimeChanging)
        ScheduleTmpOldStartTime = t->startTime();

    QTime time = ui->teScheduleDep->time();
    t->setStartTime(time);
    scheduleStartTimeChanging = true;

    //undoStack->push(new cmdScheduleTripChangeStartTime(t, time));

    refreshSchedule();
    refreshTripDetails();
}

void WdgSchedule::actionScheduleTripChangeDays() {
    Trip *t = getScheduleSelectedTrip();
    if(!t)
        return;

    bool monday = ui->cbScheduleDayMon->isChecked();
    bool tuesday = ui->cbScheduleDayTue->isChecked();
    bool wednesday = ui->cbScheduleDayWed->isChecked();
    bool thursday = ui->cbScheduleDayThu->isChecked();
    bool friday = ui->cbScheduleDayFri->isChecked();
    bool saturday = ui->cbScheduleDaySat->isChecked();
    bool sunday = ui->cbScheduleDaySun->isChecked();
    bool holiday = ui->cbScheduleDayHol->isChecked();
    bool school = ui->cbScheduleDaySco->isChecked();
    bool noSchool = ui->cbScheduleDayNsco->isChecked();

    WeekDays w(
        monday,
        tuesday,
        wednesday,
        thursday,
        friday,
        saturday,
        sunday,
        holiday,
        school,
        noSchool
        );

    undoStack->push(new cmdScheduleTripChangeDays(t, w));
    refreshSchedule();
}

void WdgSchedule::actionScheduleTripSwitchRepeat() {
    bool repeatEnabled = ui->gbScheduleRepeat->isChecked();

    Trip *t = getScheduleSelectedTrip();
    if(!t)
        return;

    Trip newT = *t;
    newT.setRepeat(repeatEnabled);
    if(repeatEnabled)
        newT.setRepeatEnd(t->startTime()); // wenn das abstürzt, mal hier testweise löschen

    undoStack->push(new cmdScheduleTripEditRepeat(t, newT));
    refreshSchedule();
}

void WdgSchedule::actionScheduleTripChangeRepeat() {
    Trip *t = getScheduleSelectedTrip();
    if(!t)
        return;

    Trip newT = *t;
    newT.setRepeatInterval(ui->teScheduleRepeatInt->time());
    newT.setRepeatEnd(ui->teScheduleRepeatEnd->time());

    undoStack->push(new cmdScheduleTripEditRepeat(t, newT));
    refreshSchedule();
}

/*Line * WdgSchedule::getScheduleSelectedLine()
{
    if(!ui->twScheduleLines->currentItem())
        return nullptr;

    QString lineID = ui->twScheduleLines->currentItem()->text(0);

    return projectData.getLine(lineID);
}*/

Route * WdgSchedule::getScheduleSelectedRoute()
{
    Line *l = currentLine;

    if(!l)
        return nullptr;

    if(!ui->twScheduleRoutes->currentItem())
        return nullptr;

    QString routeID = ui->twScheduleRoutes->currentItem()->text(0);
    return l->route(routeID);
}


void WdgSchedule::on_teScheduleDep_editingFinished() {
    Trip *t = getScheduleSelectedTrip();
    if(!t)
        return;
    
    undoStack->push(new cmdScheduleTripChangeStartTime(t, ScheduleTmpOldStartTime, t->startTime()));
    scheduleStartTimeChanging = false;

    refreshSchedule();
    refreshTripDetails();
}

Trip * WdgSchedule::getScheduleSelectedTrip()
{
    if(!ui->twSchedule->item(0, ui->twSchedule->currentColumn()))
        return nullptr;

    QString tripID = ui->twSchedule->item(0, ui->twSchedule->currentColumn())->text();

    Line *l = currentLine;
    if(!l)
        return nullptr;

    return l->trip(tripID);
}

void WdgSchedule::setCurrentLine(Line *l) {
    currentLine = l;
}

void WdgSchedule::on_twSchedule_cellDoubleClicked(int row, int column)
{
    QTableWidgetItem *itmCurrent = ui->twSchedule->item(row, 0);
    QTableWidgetItem *itmNext = ui->twSchedule->item(row + 1, 0);
    if(!itmCurrent || !itmNext)
        return;

    QString currentBusstopID = itmCurrent->text();
    QString nextBusstopID = itmNext->text();
    Line *l = currentLine;
    QStringList lines = {};
    if(l)
        lines << l->id();

    QStringList directions = {};
    if(itmNext)
        directions << itmNext->text();

    /*BusstopScheduleViewer *dlg = new BusstopScheduleViewer(this, projectData, currentBusstopID, {}, directions, scheduleCurrentDaysShow);
    dlg->show();*/
}


void WdgSchedule::refreshScheduleRoutes()
{
    ui->twScheduleRoutes->clear();

    Line *l = currentLine;
    if(!l)
        return;

    for(int i = 0; i < l->routeCount(); i++) {
        Route *r = l->routeAt(i);
        
        if(r->direction() != scheduleCurrentDirection)
            continue;

        QTreeWidgetItem *itm = new QTreeWidgetItem(ui->twScheduleRoutes);
        itm->setText(0, r->id());
        itm->setText(1, r->name());
        
        if(r->profileCount() == 0) {
            itm->setDisabled(true);
            itm->setToolTip(1, "You can't use this trip now, since it has no valid time profiles!");
        }

        ui->twScheduleRoutes->addTopLevelItem(itm);
    }
}

void WdgSchedule::refreshSchedule()
{
    qDebug() << "refreshing";
    int currentScrollValue = ui->twSchedule->horizontalScrollBar()->value();
    qDebug() << "-A";

    ui->twSchedule->clearContents();
    ui->twSchedule->setColumnCount(1);
    ui->twSchedule->setRowCount(7);

    Line *l = currentLine;
    if(!l)
        return;

    QList<Trip *> trips;

    qDebug() << "-A";

    if(scheduleCurrentDirection)
        trips = l->forwardTrips();
    else
        trips = l->reverseTrips();

    qDebug() << "got trips";

    trips = scheduleExectureTripRepetitions(trips);
    qDebug() << "A";
    trips = ProjectData::sortTrips(trips);
    qDebug() << "B";

    QList<Trip *> tripsFilter;


    for(int i = 0; i < trips.count(); i++) {
        Trip *t = trips[i];
        
        if(scheduleCheckMatchingWeekdays(t->weekDays()))
            tripsFilter << t;
    }

    qDebug() << "C";

    refreshScheduleBusstopList(tripsFilter);

    qDebug() << "busstop list refreshed";


    for(int i = 0; i < tripsFilter.count(); i++)
        refreshScheduleAddTrip(tripsFilter[i]);

    qDebug() << "trips added";

    // reszie
    for(int i = 0; i <= ui->twSchedule->rowCount(); i++)
        ui->twSchedule->setRowHeight(i, 15);

    for(int i = 0; i <= ui->twSchedule->columnCount(); i++)
        ui->twSchedule->setColumnWidth(i, 50);

    //refreshScheduleSelection();

    /*if(!ui->twSchedule->currentItem())
        return;

    int column = ui->twSchedule->currentItem()->column();
    int row = ui->twSchedule->currentItem()->row();*/

    ui->twSchedule->horizontalScrollBar()->setValue(currentScrollValue);

    qDebug() << "finished";
}

void WdgSchedule::refreshSchedule(Line *l)
{
    currentLine = l;
    refreshSchedule();
    refreshScheduleRoutes();
}

QList<Trip *> WdgSchedule::scheduleExectureTripRepetitions(QList<Trip *> tl)
{
    QList<Trip *> result;
    for(int i = 0; i < tl.count(); i++) {
        Trip *t = tl[i];

        if(!t->hasRepeat()) {
            result << t;
            continue;
        }

        // if the trip has repeats...
        for(int i = 0; i < t->repeatTimes(); i++)
            result << t->repetitionAt(i);
    }

    return result;
}

bool WdgSchedule::scheduleCheckMatchingWeekdays(WeekDays *d)
{
    if(scheduleCurrentDaysShow == scheduleCurrentDaysMoFr)
        return (
            d->monday() ||
            d->tuesday() ||
            d->wednesday() ||
            d->thursday() ||
            d->friday()
            );

    else if(scheduleCurrentDaysShow == scheduleCurrentDaysSa)
        return d->saturday();

    else if(scheduleCurrentDaysShow == scheduleCurrentDaysSo)
        return (d->sunday() || d->holiday());

    return false;
}

void WdgSchedule::refreshScheduleBusstopList(QList<Trip *> trips)
{
    qDebug() << "refreshing busstop list...";
    int headerRowsCount = ui->twSchedule->rowCount();

    QList<Route *> routes;

    for(int i = 0; i < trips.count(); i++)
        routes << trips[i]->route();

    qDebug() << "got all routes...";

    // sort routes

    bool ok = false;
    while(!ok) {
        ok = true;
        for(int i = 0; i + 1 < routes.count(); i++) {
            if(routes[i]->code() <= routes[i + 1]->code())
                continue;

            Route *tmp = routes[i];

            routes[i] = routes[i + 1];
            routes[i + 1] = tmp;
            ok = false;
        }
    }

    qDebug() << "sorted all routes...";

    // for every route
    for(int i = 0; i < routes.count(); i++) {
        Route *r = routes[i];
        QString lastBusstopID = "";

        bool firstBusstopFound = false;

        // for every busstop
        for(int j = 0; j < r->busstopCount(); j++) {
            Busstop *b = r->busstopAt(j);

            if(ui->cbScheduleOnlyImportantBusstops->isChecked() && !b->isImportant())
                continue;

            bool nameMatches = b->name().contains(ui->leScheduleSearchBusstop->text(), Qt::CaseInsensitive);
            if(!nameMatches)
                continue;

            QString busstopID = b->id();

            bool busstopAlreadyExists = false;

            for(int k = headerRowsCount - 1; k < ui->twSchedule->rowCount(); k++) {
                if(!ui->twSchedule->item(k, 0))
                    continue;

                if(ui->twSchedule->item(k, 0)->text() == busstopID) {
                    busstopAlreadyExists = true;
                    lastBusstopID = busstopID;
                    firstBusstopFound = true;
                    break;
                }
            }

            if(busstopAlreadyExists)
                continue;

            QString busstopName = b->name();

            QTableWidgetItem *itmID = new QTableWidgetItem(busstopID);
            QTableWidgetItem *itmName = new QTableWidgetItem(busstopName);

            int targetRow = firstBusstopFound ? ui->twSchedule->rowCount() : headerRowsCount;

            for(int k = headerRowsCount - 1; k < ui->twSchedule->rowCount(); k++) {
                if(!ui->twSchedule->item(k, 0))
                    continue;

                if(ui->twSchedule->item(k, 0)->text() == lastBusstopID) {
                    targetRow = k + 1;
                    break;
                }
            }

            ui->twSchedule->insertRow(targetRow);
            ui->twSchedule->setVerticalHeaderItem(targetRow, itmName);
            ui->twSchedule->setItem(targetRow, 0, itmID);

            lastBusstopID = busstopID;
        }
    }

    qDebug() << "finished!";
}

void WdgSchedule::refreshScheduleAddTrip(Trip *t)
{
    int targetColumn = ui->twSchedule->columnCount();
    ui->twSchedule->insertColumn(targetColumn);

    QString id = t->id();
    QString idC = t->selfChildId();
    QString routeName = t->route()->name();
    QString timeProfile = t->timeProfile()->name();
    QString startTime = t->startTime().toString("h:mm");
    
    int timeProfileIndex = t->route()->indexOfTimeProfile(t->timeProfile());

    bool weekDaysMatch = !(
        (scheduleCurrentDaysShow == 1 && t->weekDays()->toCode() != WeekDays::MonFri) ||
        (scheduleCurrentDaysShow == 2 && t->weekDays()->toCode() != WeekDays::Sat) ||
        (scheduleCurrentDaysShow == 3 && t->weekDays()->toCode() != WeekDays::Sun)
        );

    QString info;
    QString infoEnh;

    QTableWidgetItem *itmInfo = new QTableWidgetItem;
    if(weekDaysMatch) {
        info = "";
    } else {
        QPair<QString, QString> p = refreshScheduleGenerateInfo(t->weekDays());
        info = p.first;
        infoEnh = p.second;
        itmInfo->setBackground(Qt::yellow);
    }

    itmInfo->setText(info);
    itmInfo->setToolTip(infoEnh);

    QList<Tour *> tours = projectData->toursOfTrip(t);
    QString tourInfo;
    QString toursToolTip;
    if(tours.isEmpty()) {
        tourInfo = "None";
    } else {
        QStringList strList;
        QStringList strListToolTip;
        for(int i = 0; i < tours.count(); i++) {
            strList << tours[i]->name();
            strListToolTip << "- " + tours[i]->name() + " (" + tours[i]->weekDays()->toString() + ")";
        }
        tourInfo = strList.join(", ");
        toursToolTip = strListToolTip.join("\r\n");
    }
    QList<WeekDays> tourWeekDays;
    for(int i = 0; i < tours.count(); i++)
        tourWeekDays << *tours[i]->weekDays();

    ui->twSchedule->setItem(0, targetColumn, new QTableWidgetItem(id));
    ui->twSchedule->setItem(1, targetColumn, new QTableWidgetItem(idC));
    ui->twSchedule->setItem(2, targetColumn, new QTableWidgetItem(tourInfo));
    ui->twSchedule->setItem(3, targetColumn, itmInfo);
    ui->twSchedule->setItem(4, targetColumn, new QTableWidgetItem(routeName));
    ui->twSchedule->setItem(5, targetColumn, new QTableWidgetItem(timeProfile));
    ui->twSchedule->setItem(6, targetColumn, new QTableWidgetItem(startTime));

    QFont bold;
    bold.setBold(true);

    QList<QColor> profileColors = {QColor("#00a3ff"), QColor("#b900ff"), QColor("#ff00b8"), QColor("#ff8600"), QColor("#80ff00")};

    if(tours.empty()) {
        ui->twSchedule->item(2, targetColumn)->setBackground(Qt::red);
        ui->twSchedule->item(2, targetColumn)->setForeground(Qt::yellow);
    } else {

        WeekDays combinedTourWeekDays = WeekDays::combine(tourWeekDays);
        bool match = combinedTourWeekDays.toCode() == t->weekDays()->toCode();
        bool overlap = WeekDays::overlap(tourWeekDays);
        QString overlapStr = WeekDays::intersection(tourWeekDays).toString();


        if(match && !overlap) {
            ui->twSchedule->item(2, targetColumn)->setBackground(Qt::darkGreen);
            ui->twSchedule->item(2, targetColumn)->setForeground(Qt::white);
        } else if(overlap) {
            ui->twSchedule->item(2, targetColumn)->setBackground(QColor("#ffa800"));
            ui->twSchedule->item(2, targetColumn)->setForeground(Qt::black);
            toursToolTip = "Multiple assignment at " + overlapStr + ":\r\n\r\n" + toursToolTip;
        } else {
            ui->twSchedule->item(2, targetColumn)->setBackground(Qt::blue);
            ui->twSchedule->item(2, targetColumn)->setForeground(Qt::white);
        }
    }

    ui->twSchedule->item(2, targetColumn)->setToolTip(toursToolTip);
    ui->twSchedule->item(4, targetColumn)->setToolTip(routeName);

    QColor currentProfileColor = (timeProfileIndex >= 4 || timeProfileIndex < 0) ? profileColors[4] : profileColors[timeProfileIndex];
    ui->twSchedule->item(5, targetColumn)->setBackground(currentProfileColor);
    ui->twSchedule->item(5, targetColumn)->setForeground(global::getContrastColor(currentProfileColor));
    ui->twSchedule->item(5, targetColumn)->setToolTip(timeProfile);

    ui->twSchedule->item(6, targetColumn)->setFont(bold);
    ui->twSchedule->item(6, targetColumn)->setBackground(QColor("#555555"));
    ui->twSchedule->item(6, targetColumn)->setForeground(Qt::white);

    int lastBusstopFound = -1;
    for(int i = 7; i < ui->twSchedule->rowCount(); i++) {
        if(!ui->twSchedule->item(i, 0))
            continue;

        QString busstopID = ui->twSchedule->item(i, 0)->text();
        TimeProfileItem *itm = t->timeProfile()->busstop(busstopID);

        if(!itm)
            continue;

        if(lastBusstopFound != -1) {
            for(int j = lastBusstopFound + 1; j < i; j++) {
                QTableWidgetItem *fillItm = new QTableWidgetItem("|");
                ui->twSchedule->setItem(j, targetColumn, fillItm);
            }
        }

        QTableWidgetItem *tableItm = new QTableWidgetItem;
        lastBusstopFound = i;
        float depVal = itm->depValue();
        QString timeStr = t->startTime().addSecs(depVal * 60).toString("h:mm");
        tableItm->setText(timeStr);
        
        if(projectData->busstop(busstopID)->isImportant())
            tableItm->setFont(bold);

        ui->twSchedule->setItem(i, targetColumn, tableItm);
    }

    for(int i = 0; i < ui->twSchedule->rowCount(); i++) {
        QTableWidgetItem *itm = ui->twSchedule->item(i, targetColumn);
        if(!itm) {
            itm = new QTableWidgetItem;
            ui->twSchedule->setItem(i, targetColumn, itm);
        }

        itm->setTextAlignment(Qt::AlignCenter);
        if((i % 2) == 0 && i >= 7)
            itm->setBackground(QColor(224, 224, 224));
    }


    if(id == scheduleCurrentTripId && ui->twSchedule->currentItem() == nullptr) {
        ui->twSchedule->setCurrentCell(0, targetColumn);
        //ui->twSchedule->setRangeSelected(QTableWidgetSelectionRange(0, targetColumn, ui->twSchedule->rowCount() - 1, targetColumn), true);
    }
}

QPair<QString, QString> WdgSchedule::refreshScheduleGenerateInfo(WeekDays *w)
{
    bool monday = w->monday();       // 0
    bool tuesday = w->tuesday();     // 1
    bool wednesday = w->wednesday(); // 2
    bool thursday = w->thursday();   // 3
    bool friday = w->friday();       // 4
    bool saturday = w->saturday();   // 5
    bool sunday = w->sunday();       // 6
    bool holiday = w->holiday();     // 7
    bool school = w->school();       // 8
    bool noSchool = w->noSchool();   // 9

    QList<int> addList;
    QList<int> onlyList;
    QList<int> notList;

    if(scheduleCurrentDaysShow == scheduleCurrentDaysMoFr) {
        if(saturday) addList << 5;
        if(sunday) addList << 6;
        if(holiday) addList << 7;
    }

    if(scheduleCurrentDaysShow == scheduleCurrentDaysSa || scheduleCurrentDaysShow == scheduleCurrentDaysSo) {
        if(monday) addList << 0;
        if(tuesday) addList << 1;
        if(wednesday) addList << 2;
        if(thursday) addList << 3;
        if(friday) addList << 4;
    }

    if(scheduleCurrentDaysShow == scheduleCurrentDaysSa) {
        if(sunday) addList << 6;
        if(holiday) addList << 7;
    }

    if(scheduleCurrentDaysShow == scheduleCurrentDaysSo) {
        if(saturday) addList << 5;
    }


    if(scheduleCurrentDaysShow == scheduleCurrentDaysMoFr) {
        if(!monday) notList << 0;
        if(!tuesday) notList << 1;
        if(!wednesday) notList << 2;
        if(!thursday) notList << 3;
        if(!friday) notList << 4;

        if(notList.count() > 3) {
            notList.clear();
            if(monday) onlyList << 0;
            if(tuesday) onlyList << 1;
            if(wednesday) onlyList << 2;
            if(thursday) onlyList << 3;
            if(friday) onlyList << 4;
        }
    }

    if(scheduleCurrentDaysShow == scheduleCurrentDaysSa) {
        if(!saturday) notList << 5;
    }

    if(scheduleCurrentDaysShow == scheduleCurrentDaysSo) {
        if(!sunday) notList << 6;
        if(!holiday) notList << 7;
    }

    QStringList addStrListLong;
    QStringList onlyStrListLong;
    QStringList notStrListLong;

    QStringList addStrListShort;
    QStringList onlyStrListShort;
    QStringList notStrListShort;

    for(int i = 0; i < addList.count(); i++) {
        if(addList[i] == 0) { addStrListLong << "Monday"   ; addStrListShort << "+Mon"; }
        if(addList[i] == 1) { addStrListLong << "Tuesday"  ; addStrListShort << "+Tue"; }
        if(addList[i] == 2) { addStrListLong << "Wednesday"; addStrListShort << "+Wed"; }
        if(addList[i] == 3) { addStrListLong << "Thursday" ; addStrListShort << "+Thu"; }
        if(addList[i] == 4) { addStrListLong << "Friday"   ; addStrListShort << "+Fri"; }
        if(addList[i] == 5) { addStrListLong << "Saturday" ; addStrListShort << "+Sat"; }
        if(addList[i] == 6) { addStrListLong << "Sunday"   ; addStrListShort << "+Sun"; }
        if(addList[i] == 7) { addStrListLong << "Holiday"  ; addStrListShort << "+Hol"; }
    }

    for(int i = 0; i < onlyList.count(); i++) {
        if(onlyList[i] == 0) { onlyStrListLong << "Monday"   ; onlyStrListShort << "Mon"; }
        if(onlyList[i] == 1) { onlyStrListLong << "Tuesday"  ; onlyStrListShort << "Tue"; }
        if(onlyList[i] == 2) { onlyStrListLong << "Wednesday"; onlyStrListShort << "Wed"; }
        if(onlyList[i] == 3) { onlyStrListLong << "Thursday" ; onlyStrListShort << "Thu"; }
        if(onlyList[i] == 4) { onlyStrListLong << "Friday"   ; onlyStrListShort << "Fri"; }
        if(onlyList[i] == 5) { onlyStrListLong << "Saturday" ; onlyStrListShort << "Sat"; }
        if(onlyList[i] == 6) { onlyStrListLong << "Sunday"   ; onlyStrListShort << "Sun"; }
        if(onlyList[i] == 7) { onlyStrListLong << "Holiday"  ; onlyStrListShort << "Hol"; }
    }

    for(int i = 0; i < notList.count(); i++) {
        if(notList[i] == 0) { notStrListLong << "Monday"   ; notStrListShort << "-Mon"; }
        if(notList[i] == 1) { notStrListLong << "Tuesday"  ; notStrListShort << "-Tue"; }
        if(notList[i] == 2) { notStrListLong << "Wednesday"; notStrListShort << "-Wed"; }
        if(notList[i] == 3) { notStrListLong << "Thursday" ; notStrListShort << "-Thu"; }
        if(notList[i] == 4) { notStrListLong << "Friday"   ; notStrListShort << "-Fri"; }
        if(notList[i] == 5) { notStrListLong << "Saturday" ; notStrListShort << "-Sat"; }
        if(notList[i] == 6) { notStrListLong << "Sunday"   ; notStrListShort << "-Sun"; }
        if(notList[i] == 7) { notStrListLong << "Holiday"  ; notStrListShort << "-Hol"; }
    }

    QString schoolStrLong;
    QString schoolStrShort;
    if(school && noSchool) {
        schoolStrLong = "";
        schoolStrShort = "";
    }
    else if(!school && !noSchool) {
        schoolStrLong = "NEVER";
        schoolStrShort = "X";
    }
    else if(school && !noSchool) {
        schoolStrLong = "only School";
        schoolStrShort = "S";
    }
    else if(!school && noSchool) {
        schoolStrLong = "only Vacation";
        schoolStrShort = "V";
    }

    QString longStr = schoolStrLong;
    QString shortStr = schoolStrShort.isEmpty() ? QStringList(notStrListShort << onlyStrListShort << addStrListShort).join(", ") : schoolStrShort + (QStringList(notStrListShort << onlyStrListShort << addStrListShort).empty() ? "" : "; ") + QStringList(notStrListShort << onlyStrListShort << addStrListShort).join(", ");

    if(notList.count() == 0 && onlyList.count() == 0 && addList.count() != 0) {
        longStr += (longStr.isEmpty() ? QString("") : QString("; ")) + "additionally " + addStrListLong.join(", ");
    } else if(notList.count() == 0 && (onlyList.count() != 0  || addList.count() != 0)) {
        longStr += (longStr.isEmpty() ? QString("") : QString("; ")) + "only " + QStringList(onlyStrListLong << addStrListLong).join(", ");
    } else if(onlyList.count() == 0 && (notList.count() != 0)) {
        longStr += (longStr.isEmpty() ? QString("") : QString("; ")) + "not " + notStrListLong.join(", ");
        if(!addList.empty()) {
            longStr += "; but " + addStrListLong.join(", ");
        }
    }

    QPair<QString, QString> p;
    p.first = shortStr;
    p.second = longStr;
    return p;
}

void WdgSchedule::refreshTripDetails()
{
    ui->cbScheduleProfiles->clear();
    ui->twScheduleRoutes->clearSelection();

    Trip *t = getScheduleSelectedTrip();
    if(!t)
        return;

    scheduleCurrentTripId = t->id();
    
    Route *r = t->route();

    // select route
    for(int i = 0; i < ui->twScheduleRoutes->topLevelItemCount(); i++) {
        if(ui->twScheduleRoutes->topLevelItem(i)->text(0) == r->id()) {
            ui->twScheduleRoutes->topLevelItem(i)->setSelected(true);
            break;
        }
    }
    
    for(int i = 0; i < r->profileCount(); i++) {
        ui->cbScheduleProfiles->addItem(r->timeProfileAt(i)->name());
        if(r->timeProfileAt(i) == t->timeProfile())
            ui->cbScheduleProfiles->setCurrentIndex(i);
    }
    
    ui->teScheduleDep->setTime(t->startTime());
    ui->lScheduleArr->setText(t->endTime().toString("h:mm:ss"));
    
    WeekDays *d = t->weekDays();
    
    ui->cbScheduleDayMon->setChecked(d->monday());
    ui->cbScheduleDayTue->setChecked(d->tuesday());
    ui->cbScheduleDayWed->setChecked(d->wednesday());
    ui->cbScheduleDayThu->setChecked(d->thursday());
    ui->cbScheduleDayFri->setChecked(d->friday());
    ui->cbScheduleDaySat->setChecked(d->saturday());
    ui->cbScheduleDaySun->setChecked(d->sunday());
    ui->cbScheduleDayHol->setChecked(d->holiday());
    ui->cbScheduleDaySco->setChecked(d->school());
    ui->cbScheduleDayNsco->setChecked(d->noSchool());

    if(t->hasRepeat()) {
        ui->gbScheduleRepeat->setChecked(true);
        ui->teScheduleRepeatInt->setTime(t->repeatInterval());
        ui->teScheduleRepeatEnd->setTime(t->repeatEnd());
    } else {
        ui->gbScheduleRepeat->setChecked(false);
        ui->teScheduleRepeatInt->setTime(QTime(0, 10, 0, 0));
        ui->teScheduleRepeatEnd->setTime(t->startTime());
    }
}

void WdgSchedule::refreshScheduleSelection()
{
    Trip *t = getScheduleSelectedTrip();
    if(!t)
        return;

    int rows = ui->twSchedule->rowCount();
    int columns = ui->twSchedule->columnCount();

    // alle Spalten deselektieren
    ui->twSchedule->setRangeSelected(QTableWidgetSelectionRange(0, 0, rows - 1, columns - 1), false);

    QTableWidgetItem *sitm = ui->twSchedule->currentItem();
    // ausgewählte wieder selektieren
    for(int i = 0; i < columns; i++) {
        if(sitm && sitm->row() != 0 && sitm->column() == i)
            continue;

        QTableWidgetItem *itm = ui->twSchedule->item(0, i);
        if(!itm)
            continue;
        QString id = itm->text();
        if(id == t->id()) {
            ui->twSchedule->setRangeSelected(QTableWidgetSelectionRange(0, i, rows - 1, i), true);
        }
    }
}


/*void WdgSchedule::on_twScheduleLines_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(!current)
        return;

    scheduleCurrentLineId = current->text(0);
    refreshScheduleRoutes();
    refreshSchedule();
}*/

void WdgSchedule::on_twSchedule_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    qDebug() << "ok";
    refreshTripDetails();
    refreshScheduleSelection();
}
