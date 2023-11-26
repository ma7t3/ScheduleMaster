#include "wdgschedule.h"
#include "ui_wdgschedule.h"

#include <QMessageBox>
#include <QScrollBar>

#include "App/global.h"
#include "ProjectData/projectdata.h"
#include "Commands/cmdschedule.h"

WdgSchedule::WdgSchedule(QWidget *parent, ProjectData *projectData, QUndoStack *undoStack) :
    QWidget(parent),
    ui(new Ui::WdgSchedule),
    projectData(projectData),
    undoStack(undoStack),
    m_currentLine(nullptr)
{
    ui->setupUi(this);

    /*QObject::connect(ui->pbForwardTrip, SIGNAL(clicked()), this, SLOT(actionScheduleShowTripForward()));
    QObject::connect(ui->pbReverseTrip, SIGNAL(clicked()), this, SLOT(actionScheduleShowTripReverse()));*/
    QObject::connect(ui->cmbDirections, SIGNAL(activated(int)), this, SLOT(actionChangeDirection()));
    QObject::connect(ui->leSearchBusstop, SIGNAL(textChanged(QString)), this, SLOT(refreshSchedule()));
    QObject::connect(ui->cbOnlyImportantBusstops, SIGNAL(stateChanged(int)), this, SLOT(refreshSchedule()));

    QObject::connect(ui->pbTripNew, SIGNAL(clicked()), this, SLOT(actionScheduleTripNew()));
    QObject::connect(ui->pbTripDelete, SIGNAL(clicked()), this, SLOT(actionScheduleTripDelete()));
    QObject::connect(ui->lwRoutes, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(actionScheduleTripChangeRoute()));
    QObject::connect(ui->cbTimeProfiles, SIGNAL(activated(int)), this, SLOT(actionScheduleTripChangeProfile()));
    QObject::connect(ui->teDepartureTime, SIGNAL(userTimeChanged(QTime)), this, SLOT(actionScheduleTripChangeStartTime()));

    QObject::connect(ui->cbDayMon, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbDayTue, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbDayWed, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbDayThu, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbDayFri, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbDaySat, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbDaySun, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbDayHol, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbDaySco, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));
    QObject::connect(ui->cbDayNsco, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));

    ui->twSchedule->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->twSchedule->horizontalHeader()->setVisible(false);
    ui->twSchedule->setRowHidden(0, true);
}

WdgSchedule::~WdgSchedule()
{
    delete ui;
}

void WdgSchedule::actionChangeDirection()
{
    m_currentLineDirection = lineDirectionsReference[ui->cmbDirections->currentIndex()];
    refreshRoutes();
    refreshSchedule();
}

void WdgSchedule::actionScheduleTripNew() {
    if(!m_currentLine || !m_currentRoute)
        return;

    TimeProfile *p = m_currentRoute->timeProfileAt(0);
    if(!p)
        return;

    QTime time = ui->teDepartureTime->time();
    if(!time.isValid())
        time.setHMS(0, 0, 0, 0);

    Trip *t = new Trip(global::getNewID(), m_currentRoute, time, p);

    WeekDays *d = t->weekDays();

    d->setCode(m_currentDayType ? m_currentDayType->toCode() : DayType::MonFri);

    /*if(scheduleCurrentDaysShow == 1)
        d->setCode(WeekDays::MonFri);
    else if(scheduleCurrentDaysShow == 2)
        d->setCode(WeekDays::Sat);
    else if(scheduleCurrentDaysShow == 3)
        d->setCode(WeekDays::Sun);*/
    undoStack->push(new cmdScheduleTripNew(m_currentLine, t, m_currentLineDirection));
    m_currentTrip = t;
    refreshSchedule();
}

void WdgSchedule::actionScheduleTripDelete() {
    if(!m_currentLine || !m_currentTrip)
        return;

    QString tripName    = m_currentTrip->route()->name();
    QString startTime   = m_currentTrip->startTime().toString("hh:mm");
    QString endTime     = m_currentTrip->endTime().toString("hh:mm");
    QString firstStop   = m_currentTrip->route()->busstopAt(0)->name();
    QString lastStop    = m_currentTrip->route()->busstopAt(m_currentTrip->route()->busstopCount() - 1)->name();
    QString repeat      = tr("No");

    QString msgStr = tr("<p><b>Do you really want do delete this trip?</b></p><table><tr><td><b>Name:</b></td><td colspan=\"2\">%1</td></tr><tr><td><b>Start:</b></td><td>%2</td><td>%3</td></tr><tr><td><b>End:</b></td><td>%4</td><td>%5</td></tr><tr><td><b>Repeat:</b></td><td colspan=\"2\">%6</td></tr></table>").arg(tripName, startTime, firstStop, endTime, lastStop, repeat);

    QMessageBox::StandardButton msg = QMessageBox::warning(this, "deleteTrip", msgStr, QMessageBox::Yes|QMessageBox::No);

    if(msg != QMessageBox::Yes)
        return;

    undoStack->push(new cmdScheduleTripDelete(m_currentLine, m_currentTrip));
    m_currentTrip = nullptr;
    refreshSchedule();
}

void WdgSchedule::actionScheduleTripChangeRoute() {
    if(!m_currentTrip || !m_currentRoute)
        return;

    if(m_currentRoute->profileCount() == 0)
        return;

    QString targetName = m_currentTrip->timeProfile()->name();

    TimeProfile *newP = nullptr;
    TimeProfile *p = m_currentRoute->timeProfileWithName(targetName);
    if(p)
        newP = p;
    else
        newP = m_currentRoute->timeProfileAt(0);

    undoStack->push(new cmdScheduleTripChangeRoute(m_currentTrip, m_currentRoute, newP));

    refreshSchedule();
}

void WdgSchedule::actionScheduleTripChangeProfile() {
    if(!m_currentTrip)
        return;

    int index = ui->cbTimeProfiles->currentIndex();

    Route *r = m_currentTrip->route();
    TimeProfile *p = r->timeProfileAt(index);
    if(!p)
        return;

    undoStack->push(new cmdScheduleTripChangeTimeProfile(m_currentTrip, p));

    refreshSchedule();
}

void WdgSchedule::actionScheduleTripChangeStartTime() {
    if(!m_currentTrip)
        return;

    if(ui->twSchedule->hasFocus())
        return;

    if(!ui->teDepartureTime->time().isValid())
        return;

    if(!scheduleStartTimeChanging)
        ScheduleTmpOldStartTime = m_currentTrip->startTime();

    QTime time = ui->teDepartureTime->time();
    m_currentTrip->setStartTime(time);
    scheduleStartTimeChanging = true;
    refreshSchedule();

    //undoStack->push(new cmdScheduleTripChangeStartTime(currentTrip, currentTrip->startTime(),time));
}

void WdgSchedule::actionScheduleTripChangeDays() {
    if(!m_currentTrip)
        return;

    bool monday = ui->cbDayMon->isChecked();
    bool tuesday = ui->cbDayTue->isChecked();
    bool wednesday = ui->cbDayWed->isChecked();
    bool thursday = ui->cbDayThu->isChecked();
    bool friday = ui->cbDayFri->isChecked();
    bool saturday = ui->cbDaySat->isChecked();
    bool sunday = ui->cbDaySun->isChecked();
    bool holiday = ui->cbDayHol->isChecked();
    bool school = ui->cbDaySco->isChecked();
    bool noSchool = ui->cbDayNsco->isChecked();

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

    undoStack->push(new cmdScheduleTripChangeDays(m_currentTrip, w));
    refreshSchedule();
}

/*void WdgSchedule::actionScheduleTripSwitchRepeat() {
    if(!m_currentTrip)
        return;

    bool repeatEnabled = ui->gbRepeat->isChecked();

    Trip newT = *m_currentTrip;
    newT.setRepeat(repeatEnabled);
    if(repeatEnabled)
        newT.setRepeatEnd(m_currentTrip->startTime());

    undoStack->push(new cmdScheduleTripEditRepeat(m_currentTrip, newT));

    refreshSchedule();
}

void WdgSchedule::actionScheduleTripChangeRepeat() {
    if(!m_currentTrip)
        return;

    Trip newT = *m_currentTrip;
    newT.setRepeatInterval(ui->teRepeatInterval->time());
    newT.setRepeatEnd(ui->teRepeatEnd->time());

    undoStack->push(new cmdScheduleTripEditRepeat(m_currentTrip, newT));

    refreshSchedule();
}*/

void WdgSchedule::setCurrentLine(Line *l) {
    m_currentLine = l;
    refreshDirections();
    refreshRoutes();
    refreshSchedule();
    refreshTripDetails();
}

void WdgSchedule::refreshDirections() {
    int lastIndex = ui->cmbDirections->currentIndex();

    ui->cmbDirections->clear();
    lineDirectionsReference.clear();

    if(!m_currentLine)
        return;

    for(int i = 0; i < m_currentLine->directionCount(); i++) {
        LineDirection *ld = m_currentLine->directionAt(i);
        lineDirectionsReference << ld;
        ui->cmbDirections->addItem(ld->description());
    }

    if(lineDirectionsReference.count() > lastIndex)
        ui->cmbDirections->setCurrentIndex(lastIndex);
    else
        ui->cmbDirections->setCurrentIndex(ui->cmbDirections->count() - 1);

    if(lastIndex == -1)
        ui->cmbDirections->setCurrentIndex(0);

    m_currentLineDirection = lineDirectionsReference[ui->cmbDirections->currentIndex()];
}

void WdgSchedule::refreshDayTypes() {
    dayTypesReference = projectData->projectSettings()->dayTypes();

    ui->cmbDayTypes->clear();
    for(int i = 0; i < dayTypesReference.count(); i++)
        ui->cmbDayTypes->addItem(dayTypesReference[i]->name());

    if(!dayTypesReference.empty()) {
        ui->cmbDayTypes->setCurrentIndex(0);
        m_currentDayType = dayTypesReference[0];
    }
}

void WdgSchedule::refreshRoutes() {
    ui->lwRoutes->clear();
    routeTableReference.clear();

    if(!m_currentLine)
        return;

    QList<Route *> routes = m_currentLine->routes();
    routes = ProjectData::sortRoutes(routes);

    for(int i = 0; i < routes.count(); i++) {
        Route *r = routes[i];

        if(r->direction() != m_currentLineDirection)
            continue;

        routeTableReference << r;

        QListWidgetItem *itm = new QListWidgetItem(r->name());

        if(r->profileCount() == 0) {
            itm->setHidden(true);
            itm->setToolTip(tr("You can't use this trip now, since it has no valid time profiles!"));
        }
        ui->lwRoutes->addItem(itm);
    }
}

void WdgSchedule::refreshSchedule() {
    int currentScrollValue = ui->twSchedule->horizontalScrollBar()->value();

    ui->twSchedule->clearContents();
    ui->twSchedule->setColumnCount(0);
    ui->twSchedule->setRowCount(headerRowCount);

    scheduleTableTripsReference.clear();

    if(!m_currentLine)
        return;

    QList<Trip *> trips, filteredTrips;

    trips = m_currentLine->tripsToDirection(m_currentLineDirection);

    /*if(scheduleCurrentDirection)
        trips = m_currentLine->forwardTrips();
    else
        trips = m_currentLine->reverseTrips();*/

    for(int i = 0; i < trips.count(); i++) {
        Trip *t = trips[i];

        if(scheduleCheckMatchingWeekdays(t->weekDays())) {
            filteredTrips << t;
        }
    }

    //trips = filteredTrips;

    refreshScheduleBusstopList(trips);

    // get all repetitions and sort trips
    //filteredTrips = exectueTripRepetitions(filteredTrips);
    filteredTrips = ProjectData::sortTrips(filteredTrips);

    for(int i = 0; i < filteredTrips.count(); i++)
        refreshScheduleAddTrip(filteredTrips[i]);

    // resize columns and rows
    for(int i = 0; i <= ui->twSchedule->rowCount(); i++)
        ui->twSchedule->setRowHeight(i, 15);

    for(int i = 0; i <= ui->twSchedule->columnCount(); i++)
        ui->twSchedule->setColumnWidth(i, 50);

    refreshScheduleSelection();

    if(ui->twSchedule->selectedItems().count() == 0) {
        m_currentTrip = nullptr;
    }

    ui->twSchedule->horizontalScrollBar()->setValue(currentScrollValue);
}

void WdgSchedule::refreshScheduleBusstopList(QList<Trip *> trips) {
    scheduleTableBusstopsReference.clear();

    QList<Route *> routes;
    for(int i = 0; i < trips.count(); i++) {
        Route *r = trips[i]->route();
        if(!routes.contains(r))
            routes << r;
    }

    routes = ProjectData::sortRoutes(routes);

    for(int i = 0; i < routes.count(); i++) {
        Route *r = routes[i];
        Busstop *lastBusstop = nullptr;
        bool firstBusstopFound = false;

        for(int j = 0; j < r->busstopCount(); j++) {
            Busstop *b = r->busstopAt(j);

            // skip if not important, but only important busstops should be shown
            if(ui->cbOnlyImportantBusstops->isChecked() && !b->isImportant())
                continue;

            // skip if there was searched for a certain name and current busstop doesn't match
            bool nameMatches = b->name().contains(ui->leSearchBusstop->text(), Qt::CaseInsensitive);
            if(!nameMatches)
                continue;

            // determine, if busstop was already added
            bool busstopAlreadyExists = false;
            for(int k = 0; k < scheduleTableBusstopsReference.count(); k++) {
                if(scheduleTableBusstopsReference[k] == b) {
                    busstopAlreadyExists = true;
                    firstBusstopFound = true;
                    lastBusstop = b;
                    break;
                }
            }

            if(busstopAlreadyExists)
                continue;

            int targetIndex;
            if(firstBusstopFound) {
                targetIndex = scheduleTableBusstopsReference.count();
            } else {
                targetIndex = 0;
            }

            for(int k = 0; k < scheduleTableBusstopsReference.count(); k++) {
                if(scheduleTableBusstopsReference[k] == lastBusstop) {
                    targetIndex = k + 1;
                    break;
                }
            }

            ui->twSchedule->insertRow(targetIndex + headerRowCount);
            ui->twSchedule->setVerticalHeaderItem(targetIndex + headerRowCount, new QTableWidgetItem(b->name()));
            scheduleTableBusstopsReference.insert(targetIndex, b);
            lastBusstop = b;
        }
    }
}

void WdgSchedule::refreshScheduleAddTrip(Trip *t) {
    if(!t)
        return;

    Trip *parentT = projectData->trip(t->id());
    if(parentT)
        scheduleTableTripsReference << parentT;

    QFont bold;
    bold.setBold(true);

    int targetColumn = ui->twSchedule->columnCount();
    ui->twSchedule->insertColumn(targetColumn);

    QString tour, tourToolTip, info, infoToolTip;

    QTableWidgetItem *itmTour = new QTableWidgetItem(tour);
    QTableWidgetItem *itmInfo = new QTableWidgetItem(info);
    QTableWidgetItem *itmRoute = new QTableWidgetItem(t->route()->name());
    QTableWidgetItem *itmTimeProfile = new QTableWidgetItem(t->timeProfile()->name());
    QTableWidgetItem *itmStartTime = new QTableWidgetItem(t->startTime().toString("hh:mm"));

    //------------------------------

    QList<Tour *> tours = projectData->toursOfTrip(t);
    if(tours.isEmpty()) {
        tour = tr("None");
    } else {
        QStringList strList;
        QStringList strListToolTip;
        for(int i = 0; i < tours.count(); i++) {
            strList << tours[i]->name();
            strListToolTip << "- " + tours[i]->name() + " (" + tours[i]->weekDays()->toString() + ")";
        }
        tour = strList.join(", ");
        tourToolTip = strListToolTip.join("\r\n");
    }
    QList<WeekDays> tourWeekDays;
    for(int i = 0; i < tours.count(); i++)
        tourWeekDays << *tours[i]->weekDays();

    if(tours.empty()) {
        itmTour->setBackground(Qt::red);
        itmTour->setForeground(Qt::yellow);
    } else {
        WeekDays combinedTourWeekDays = WeekDays::combine(tourWeekDays);
        bool match = combinedTourWeekDays.toCode() == t->weekDays()->toCode();
        bool overlap = WeekDays::overlap(tourWeekDays);
        QString overlapStr = WeekDays::intersection(tourWeekDays).toString();

        if(match && !overlap) {
            itmTour->setBackground(Qt::darkGreen);
            itmTour->setForeground(Qt::white);
        } else if(overlap) {
            itmTour->setBackground(QColor("#ffa800"));
            itmTour->setForeground(Qt::black);
            tourToolTip = tr("Multiple assignment at %1:\r\n\r\n%2").arg(overlapStr, tourToolTip);
        } else {
            itmTour->setBackground(Qt::blue);
            itmTour->setForeground(Qt::white);
        }
    }

    itmTour->setText(tour);
    itmTour->setToolTip(tourToolTip);

    //------------------------------

    QPair<QString, QString> p = refreshScheduleGenerateInfo(t->weekDays());
    info = p.first;
    infoToolTip = p.second;
    itmInfo->setToolTip(infoToolTip);
    if(info != "")
        itmInfo->setBackground(Qt::yellow);

    itmInfo->setText(info);

    //------------------------------

    itmRoute->setToolTip(itmRoute->text());

    //------------------------------

    int timeProfileIndex = t->route()->indexOfTimeProfile(t->timeProfile());
    QList<QColor> profileColors = {QColor("#00a3ff"), QColor("#b900ff"), QColor("#ff00b8"), QColor("#ff8600"), QColor("#80ff00")};
    QColor currentProfileColor = (timeProfileIndex >= 4 || timeProfileIndex < 0) ? profileColors[4] : profileColors[timeProfileIndex];
    itmTimeProfile->setBackground(currentProfileColor);
    itmTimeProfile->setForeground(global::getContrastColor(currentProfileColor));
    itmTimeProfile->setToolTip(itmTimeProfile->text());


    //------------------------------

    itmStartTime->setFont(bold);
    itmStartTime->setBackground(QColor("#555555"));
    itmStartTime->setForeground(Qt::white);

    //------------------------------

    ui->twSchedule->setItem(0, targetColumn, new QTableWidgetItem);
    ui->twSchedule->setItem(1, targetColumn, itmTour);
    ui->twSchedule->setItem(2, targetColumn, itmInfo);
    ui->twSchedule->setItem(3, targetColumn, itmRoute);
    ui->twSchedule->setItem(4, targetColumn, itmTimeProfile);
    ui->twSchedule->setItem(5, targetColumn, itmStartTime);

    //--------------------------------------------------------------------------------

    int lastBusstopFound = -1;
    for(int i = headerRowCount; i < ui->twSchedule->rowCount(); i++) { // for each row in table
        Busstop *b = scheduleTableBusstopsReference[i - headerRowCount];
        TimeProfileItem *itm = t->timeProfile()->busstop(b);

        // route does not contain busstop, skip
        if(!itm)
            continue;

        // if already one busstop was added, fill the last missed fields with "|"
        if(lastBusstopFound != -1) {
            for(int j = lastBusstopFound + 1; j < i; j++) {
                QTableWidgetItem *fillItm = new QTableWidgetItem("|");
                ui->twSchedule->setItem(j, targetColumn, fillItm);
            }
        }

        // create and setup the actual item
        QTableWidgetItem *tableItm = new QTableWidgetItem;
        lastBusstopFound = i;
        float depVal = itm->depValue();
        QString timeStr = t->startTime().addSecs(depVal * 60).toString("h:mm");
        tableItm->setText(timeStr);

        // make bold if busstop is important
        if(b->isImportant())
            tableItm->setFont(bold);

        // insert the item
        ui->twSchedule->setItem(i, targetColumn, tableItm);
    }

    // for each row again...
    for(int i = 0; i < ui->twSchedule->rowCount(); i++) {

        // insert empty items to every undefined cell
        QTableWidgetItem *itm = ui->twSchedule->item(i, targetColumn);
        if(!itm) {
            itm = new QTableWidgetItem;
            ui->twSchedule->setItem(i, targetColumn, itm);
        }

        itm->setTextAlignment(Qt::AlignCenter); // make all times centered
        if((i % 2) == 0 && i >= headerRowCount) // "zebrastreifen"-look
            itm->setBackground(QColor(224, 224, 224));
    }

    if(m_currentTrip && t->id() == m_currentTrip->id() && !ui->twSchedule->currentItem()) {
        ui->twSchedule->setCurrentCell(0, targetColumn);
    }
}

QPair<QString, QString> WdgSchedule::refreshScheduleGenerateInfo(WeekDays *w) {
    QPair<QString, QString> p;
    p.first = "";
    p.second = "";
    return p;

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

    /*if(scheduleCurrentDaysShow == scheduleCurrentDaysMonFri) {
        if(saturday) addList << 5;
        if(sunday) addList << 6;
        if(holiday) addList << 7;
    }

    if(scheduleCurrentDaysShow == scheduleCurrentDaysSat || scheduleCurrentDaysShow == scheduleCurrentDaysSun) {
        if(monday) addList << 0;
        if(tuesday) addList << 1;
        if(wednesday) addList << 2;
        if(thursday) addList << 3;
        if(friday) addList << 4;
    }

    if(scheduleCurrentDaysShow == scheduleCurrentDaysSat) {
        if(sunday) addList << 6;
        if(holiday) addList << 7;
    }

    if(scheduleCurrentDaysShow == scheduleCurrentDaysSun) {
        if(saturday) addList << 5;
    }


    if(scheduleCurrentDaysShow == scheduleCurrentDaysMonFri) {
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

    if(scheduleCurrentDaysShow == scheduleCurrentDaysSat) {
        if(!saturday) notList << 5;
    }

    if(scheduleCurrentDaysShow == scheduleCurrentDaysSun) {
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
    return p;*/
}

void WdgSchedule::refreshScheduleSelection() {
    return;
    /*
    if(!m_currentTrip)
        return;

    if(!m_currentTrip->hasRepeat())
        return;

    int rows = ui->twSchedule->rowCount();
    int columns = ui->twSchedule->columnCount();

    QTableWidgetItem *sitm = ui->twSchedule->currentItem();
    for(int i = 0; i < columns; i++) {
        if(sitm && sitm->column() == i)
            continue;

        if(scheduleTableTripsReference[i] == m_currentTrip)
            ui->twSchedule->setRangeSelected(QTableWidgetSelectionRange(0, i, rows - 1, i), true);
    }*/
}

void WdgSchedule::refreshTripDetails() {
    ui->cbTimeProfiles->clear();
    ui->lwRoutes->clearSelection();

    if(!m_currentLine) {
        ui->pbTripNew->setDisabled(true);
        ui->pbTripDelete->setDisabled(true);
    } else {
        ui->pbTripNew->setEnabled(true);
        ui->pbTripDelete->setEnabled(true);
    }

    if(!m_currentTrip) {
        ui->pbTripDelete->setDisabled(true);
        ui->cbTimeProfiles->setDisabled(true);
        ui->teDepartureTime->setDisabled(true);
        ui->lArrivalTime->setDisabled(true);
        ui->lArrivalTime->setText("");
        ui->gbDays->setDisabled(true);
         return;
    } else {
        ui->pbTripDelete->setEnabled(true);
        ui->cbTimeProfiles->setEnabled(true);
        ui->teDepartureTime->setEnabled(true);
        ui->lArrivalTime->setEnabled(true);
        ui->gbDays->setEnabled(true);
    }

    if(!m_currentLine)
        return;


    Route *r = m_currentTrip->route();

    // select route
    for(int i = 0; i < ui->lwRoutes->count(); i++) {
        if(routeTableReference[i] == r) {
            ui->lwRoutes->item(i)->setSelected(true);
            break;
        }
    }

    for(int i = 0; i < r->profileCount(); i++) {
        ui->cbTimeProfiles->addItem(r->timeProfileAt(i)->name());
        if(r->timeProfileAt(i) == m_currentTrip->timeProfile())
            ui->cbTimeProfiles->setCurrentIndex(i);
    }

    ui->teDepartureTime->setTime(m_currentTrip->startTime());
    ui->lArrivalTime->setText(m_currentTrip->endTime().toString("h:mm:ss"));

    WeekDays *d = m_currentTrip->weekDays();

    ui->cbDayMon->setChecked(d->monday());
    ui->cbDayTue->setChecked(d->tuesday());
    ui->cbDayWed->setChecked(d->wednesday());
    ui->cbDayThu->setChecked(d->thursday());
    ui->cbDayFri->setChecked(d->friday());
    ui->cbDaySat->setChecked(d->saturday());
    ui->cbDaySun->setChecked(d->sunday());
    ui->cbDayHol->setChecked(d->holiday());
    ui->cbDaySco->setChecked(d->school());
    ui->cbDayNsco->setChecked(d->noSchool());

    /*if(m_currentTrip->hasRepeat()) {
        ui->gbRepeat->setChecked(true);
        ui->teRepeatInterval->setTime(m_currentTrip->repeatInterval());
        ui->teRepeatEnd->setTime(m_currentTrip->repeatEnd());
    } else {
        ui->gbRepeat->setChecked(false);
        ui->teRepeatInterval->setTime(QTime(0, 10, 0, 0));
        ui->teRepeatEnd->setTime(m_currentTrip->startTime());
    }*/
}

bool WdgSchedule::scheduleCheckMatchingWeekdays(WeekDays *d) {
    if(!m_currentDayType)
        return false;

    return WeekDays::overlap(*d, *m_currentDayType);

    /*if(scheduleCurrentDaysShow == 1) {
        return (
            d->monday() ||
            d->tuesday() ||
            d->wednesday() ||
            d->thursday() ||
            d->friday()
        );
    }

    else if(scheduleCurrentDaysShow == 2)
        return d->saturday();

    else if(scheduleCurrentDaysShow == 3)
        return (d->sunday() || d->holiday());

    return false;*/
}

/*QList<Trip *> WdgSchedule::exectueTripRepetitions(QList<Trip *> list) {
    QList<Trip *> resultList;

    for(int i = 0; i < list.count(); i++) {
        Trip *t = list[i];

        resultList << t;
        continue;

        if(!t->hasRepeat()) {

        }

        // if the trip has repeats...
        for(int i = 0; i < t->childCount(); i++)
            resultList << t->childAt(i);
    }

    return resultList;
}*/

void WdgSchedule::on_twSchedule_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    Q_UNUSED(previous);

    if(!current)
        return;

    /*if(current->row() != 0) {
        ui->twSchedule->setCurrentCell(0, current->column());
        return;
    }*/

    m_currentTrip = nullptr;
    if(!current) {
        refreshTripDetails();
        return;
    }

    Trip *t = scheduleTableTripsReference[current->column()];
    if(!t) {
        refreshTripDetails();
        return;
    }
    m_currentTrip = t;
    refreshTripDetails();
}


void WdgSchedule::on_twSchedule_itemPressed(QTableWidgetItem *item) {
    refreshScheduleSelection();
}


void WdgSchedule::on_lwRoutes_itemClicked(QListWidgetItem *item)
{
    if(!item)
        m_currentRoute = nullptr;

    Route *r = routeTableReference[ui->lwRoutes->indexFromItem(item).row()];
    //Route *r = projectData->route(item->text(0));
    m_currentRoute = r;
    actionScheduleTripChangeRoute();
}


void WdgSchedule::on_teDepartureTime_editingFinished() {
    if(!m_currentTrip)
        return;

    undoStack->push(new cmdScheduleTripChangeStartTime(m_currentTrip, ScheduleTmpOldStartTime, m_currentTrip->startTime()));
    scheduleStartTimeChanging = false;
}


void WdgSchedule::on_twSchedule_itemDoubleClicked(QTableWidgetItem *item) {
    if(!item)
        return;

    int index = item->row() - headerRowCount;
    Busstop *b = scheduleTableBusstopsReference[index];
    QList<Line *> lines = {m_currentLine};
    QList<Busstop *> busstops;
    if(scheduleTableBusstopsReference.count() > index + 1)
        busstops << scheduleTableBusstopsReference[index + 1];
    emit busstopScheduleRequested(b, busstops, lines, WeekDays::MonFri);
}

void WdgSchedule::on_cmbDayTypes_activated(int index) {
    m_currentDayType = dayTypesReference[index];
    refreshSchedule();
}

















