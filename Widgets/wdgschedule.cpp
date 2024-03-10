#include "wdgschedule.h"
#include "ui_wdgschedule.h"

#include <QMessageBox>
#include <QScrollBar>
#include <QInputDialog>

#include "App/global.h"
#include "ProjectData/projectdata.h"
#include "Commands/Cmdschedule.h"

WdgSchedule::WdgSchedule(QWidget *parent, ProjectData *projectData, QUndoStack *undoStack) :
    QWidget(parent),
    ui(new Ui::WdgSchedule),
    projectData(projectData),
    undoStack(undoStack),
    _currentLine(nullptr)
{
    ui->setupUi(this);

    /*QObject::connect(ui->pbForwardTrip, SIGNAL(clicked()), this, SLOT(actionScheduleShowTripForward()));
    QObject::connect(ui->pbReverseTrip, SIGNAL(clicked()), this, SLOT(actionScheduleShowTripReverse()));*/
    QObject::connect(ui->cmbDirections, SIGNAL(activated(int)), this, SLOT(actionChangeDirection()));
    QObject::connect(ui->leSearchBusstop, SIGNAL(textChanged(QString)), this, SLOT(refreshSchedule()));
    QObject::connect(ui->cbOnlyImportantBusstops, SIGNAL(stateChanged(int)), this, SLOT(refreshSchedule()));

    /*QObject::connect(ui->pbTripNew, SIGNAL(clicked()), this, SLOT(actionScheduleTripNew()));
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
    QObject::connect(ui->cbDayNsco, SIGNAL(clicked()), this, SLOT(actionScheduleTripChangeDays()));*/

    ui->twSchedule->setRowHidden(0, true);
}

WdgSchedule::~WdgSchedule()
{
    delete ui;
}

void WdgSchedule::actionChangeDirection()
{
    _currentLineDirection = lineDirectionsReference[ui->cmbDirections->currentIndex()];
    _currentTrips.clear();
    refreshSchedule();
    emit currentLineChanged(_currentLine, _currentLineDirection);
    emit currentTripsChanged(_currentTrips);
}

void WdgSchedule::setCurrentLine(Line *l) {
    _currentLine = l;
    _currentTrips.clear();
    refreshDirections();
    refreshSchedule();
    emit currentLineChanged(_currentLine, _currentLineDirection);
    emit currentTripsChanged(_currentTrips);
}

void WdgSchedule::refreshDirections() {
    int lastIndex = ui->cmbDirections->currentIndex();

    ui->cmbDirections->clear();
    lineDirectionsReference.clear();

    if(!_currentLine)
        return;

    for(int i = 0; i < _currentLine->directionCount(); i++) {
        LineDirection *ld = _currentLine->directionAt(i);
        lineDirectionsReference << ld;
        ui->cmbDirections->addItem(ld->description());
    }

    if(lineDirectionsReference.count() > lastIndex)
        ui->cmbDirections->setCurrentIndex(lastIndex);
    else
        ui->cmbDirections->setCurrentIndex(ui->cmbDirections->count() - 1);

    if(lastIndex == -1)
        ui->cmbDirections->setCurrentIndex(0);

    int currentIndex = ui->cmbDirections->currentIndex();

    if(!lineDirectionsReference.empty())
        _currentLineDirection = lineDirectionsReference[currentIndex];
    else
        _currentLineDirection = nullptr;
}

void WdgSchedule::refreshDayTypes() {
    dayTypesReference = projectData->projectSettings()->dayTypes();

    ui->cmbDayTypes->clear();
    for(int i = 0; i < dayTypesReference.count(); i++)
        ui->cmbDayTypes->addItem(dayTypesReference[i]->name());

    if(!dayTypesReference.empty()) {
        ui->cmbDayTypes->setCurrentIndex(0);
        _currentDayType = dayTypesReference[0];
    }
}

void WdgSchedule::refreshSchedule() {
    refreshSchedule({});
}

void WdgSchedule::refreshSchedule(QList<Trip *> currentTrips) {
    refreshingSchedule = true;

    if(!currentTrips.empty())
        _currentTrips = currentTrips;

    int currentScrollValue = ui->twSchedule->horizontalScrollBar()->value();

    ui->twSchedule->clearContents();
    ui->twSchedule->setColumnCount(0);
    ui->twSchedule->setRowCount(headerRowCount);

    scheduleTableTripsReference.clear();

    if(!_currentLine) {
        refreshingSchedule = false;
        return;
    }

    QList<Trip *> trips, filteredTrips;

    trips = _currentLine->tripsToDirection(_currentLineDirection);

    for(int i = 0; i < trips.count(); i++) {
        Trip *t = trips[i];

        if(scheduleCheckMatchingWeekdays(t->weekDays())) {
            filteredTrips << t;
        }
    }

    //trips = filteredTrips;

    refreshScheduleBusstopList(trips);

    // get all repetitions and sort trips
    filteredTrips = ProjectData::sortItems(filteredTrips);

    for(int i = 0; i < filteredTrips.count(); i++)
        refreshScheduleAddTrip(filteredTrips[i]);

    // resize columns and rows
    for(int i = 0; i <= ui->twSchedule->rowCount(); i++)
        ui->twSchedule->setRowHeight(i, 10);

    for(int i = 0; i <= ui->twSchedule->columnCount(); i++)
        ui->twSchedule->setColumnWidth(i, 50);

    if(ui->twSchedule->selectedItems().count() == 0) {
        _currentTrips = {};
    }

    ui->twSchedule->horizontalScrollBar()->setValue(currentScrollValue);

    refreshingSchedule = false;
}

void WdgSchedule::refreshScheduleBusstopList(QList<Trip *> trips) {
    scheduleTableBusstopsReference.clear();

    QList<Route *> routes;
    for(int i = 0; i < trips.count(); i++) {
        Route *r = trips[i]->route();
        if(!routes.contains(r))
            routes << r;
    }
    
    routes = ProjectData::sortItems(routes);

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

    // check if trip is current
    bool current = false;
    for(int i = 0; i < _currentTrips.count(); i++) {
        if(t == _currentTrips[i]) {
            current = true;
            break;
        }
    }

    if(current) {
        if(!ui->twSchedule->currentItem())
            ui->twSchedule->setCurrentCell(0, targetColumn);
        else
            ui->twSchedule->setRangeSelected(QTableWidgetSelectionRange(0, targetColumn, ui->twSchedule->rowCount() - 1, targetColumn), true);
    }
}

QPair<QString, QString> WdgSchedule::refreshScheduleGenerateInfo(WeekDays *w) {
    QPair<QString, QString> p;
    p.first = "";
    p.second = "";
    return p;

    /*bool monday = w->monday();       // 0
    bool tuesday = w->tuesday();     // 1
    bool wednesday = w->wednesday(); // 2
    bool thursday = w->thursday();   // 3
    bool friday = w->friday();       // 4
    bool saturday = w->saturday();   // 5
    bool sunday = w->sunday();       // 6
    bool holiday = w->holiday();     // 7
    bool school = w->school();       // 8
    bool noSchool = w->vacation();   // 9

    QList<int> addList;
    QList<int> onlyList;
    QList<int> notList;*/

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

bool WdgSchedule::scheduleCheckMatchingWeekdays(WeekDays *d) {
    if(!_currentDayType)
        return false;

    return WeekDays::overlap(*d, *_currentDayType);
}

void WdgSchedule::on_twSchedule_itemDoubleClicked(QTableWidgetItem *item) {
    if(!item)
        return;

    if(item->row() == 1) {
        int index = item->column();
        Trip *t = scheduleTableTripsReference[index];
        QList<Tour *> tours = projectData->toursOfTrip(t);

        if(tours.count() == 0)
            return;

        if(tours.count() == 1) {
            emit tourRequested(tours[0]);
        } else {
            QStringList tourNames;
            foreach (Tour *o, tours) {
                tourNames << o->name();
            }
            bool ok;
            QString result = QInputDialog::getItem(this, tr("Select tour"), tr("Select the tour you want to open:"), tourNames, 0, false, &ok);
            if(!ok)
                return;

            for (int i = 0; i < tourNames.count(); i++) {
                if(tourNames[i] == result)
                    emit tourRequested(tours[i]);
            }
        }
    } else {
        int index = item->row() - headerRowCount;
        if(index < 0 || index >= scheduleTableBusstopsReference.count())
            return;

        Busstop *b = scheduleTableBusstopsReference[index];
        QList<Route *> routes;
        for (int i = 0; i < _currentLine->routeCount(); ++i) {
            Route *r = _currentLine->routeAt(i);
            if(r->direction() == _currentLineDirection)
                routes << r;
        }
        emit busstopScheduleRequested(b, routes, 995);
    }
}

void WdgSchedule::on_cmbDayTypes_activated(int index) {
    _currentDayType = dayTypesReference[index];
    _currentTrips.clear();
    refreshSchedule();
    emit currentDayTypeChanged(*_currentDayType);
    emit currentTripsChanged(_currentTrips);
}



void WdgSchedule::on_twSchedule_itemSelectionChanged() {
    if(refreshingSchedule)
        return;

    _currentTrips.clear();

    for(int i = 0; i < scheduleTableTripsReference.count(); i++) {

        QTableWidgetItem *itm = ui->twSchedule->item(0, i);
        if(itm->isSelected())
            _currentTrips << scheduleTableTripsReference[i];
    }

    emit currentTripsChanged(_currentTrips);
}














