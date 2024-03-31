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

    QObject::connect(ui->cmbDirections, SIGNAL(activated(int)), this, SLOT(actionChangeDirection()));
    QObject::connect(ui->leSearchBusstop, SIGNAL(textChanged(QString)), this, SLOT(refreshSchedule()));
    QObject::connect(ui->cbOnlyImportantBusstops, SIGNAL(stateChanged(int)), this, SLOT(refreshSchedule()));

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
    qDebug() << "refreshing line directions in schedule";

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
    qDebug() << "refreshing day types in schedule";
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
    qDebug() << "refreshing schedule...";

    refreshingSchedule = true;

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

        if(checkMatchingWeekdays(t->weekDays())) {
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

    QString tour, tourToolTip;

    QTableWidgetItem *itmTour        = new QTableWidgetItem();
    QTableWidgetItem *itmInfo        = new QTableWidgetItem();
    QTableWidgetItem *itmFootnotes   = new QTableWidgetItem();
    QTableWidgetItem *itmRoute       = new QTableWidgetItem(t->route()->name());
    QTableWidgetItem *itmTimeProfile = new QTableWidgetItem(t->timeProfile()->name());
    QTableWidgetItem *itmStartTime   = new QTableWidgetItem(t->startTime().toString("hh:mm"));

    //------------------------------

    QList<Tour *> tours = ProjectData::sortItems(projectData->toursOfTrip(t));

    if(tours.isEmpty()) {
        tour = tr("None");
    } else {
        QStringList strList;
        QStringList strListToolTip;
        for(int i = 0; i < tours.count(); i++) {
            strList << tours[i]->name();
            strListToolTip << "- " + tours[i]->name() + " (" + tours[i]->weekDays().toString() + ")";
        }
        tour = strList.join(", ");
        tourToolTip = strListToolTip.join("\r\n");
    }
    QList<WeekDays> tourWeekDays;
    for(int i = 0; i < tours.count(); i++) {
        tourWeekDays << tours[i]->weekDays();
    }

    if(tours.empty()) {
        itmTour->setBackground(Qt::red);
        itmTour->setForeground(Qt::yellow);
    } else {
        WeekDays combinedTourWeekDays = WeekDays::combine(tourWeekDays);
        bool match = combinedTourWeekDays == t->weekDays();
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

    QList<Footnote *> footnotes = projectData->autoAssignedFootnotesOfTrip(t);
    QStringList footnotesStrList;
    QStringList footnotesToolTipStrList;
    for(int i = 0; i < footnotes.count(); i++) {
        footnotesStrList << footnotes[i]->identifier();
        footnotesToolTipStrList << footnotes[i]->identifier() + ": " + footnotes[i]->description();
    }

    itmFootnotes->setText(footnotesStrList.join(", "));
    itmFootnotes->setToolTip(footnotesToolTipStrList.join("\r\n"));

    if(!footnotesStrList.isEmpty())
        itmFootnotes->setBackground(Qt::yellow);

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
    ui->twSchedule->setItem(3, targetColumn, itmFootnotes);
    ui->twSchedule->setItem(4, targetColumn, itmRoute);
    ui->twSchedule->setItem(5, targetColumn, itmTimeProfile);
    ui->twSchedule->setItem(6, targetColumn, itmStartTime);

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

bool WdgSchedule::checkMatchingWeekdays(WeekDays d) {
    if(!_currentDayType)
        return false;

    return WeekDays::overlap(d, *_currentDayType);
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
        emit busstopScheduleRequested(b, routes, _currentDayType);
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














