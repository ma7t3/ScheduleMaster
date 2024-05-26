#include "wdgschedule.h"
#include "ui_wdgschedule.h"

#include <QMessageBox>
#include <QScrollBar>
#include <QInputDialog>

#include "App/global.h"
#include "ProjectData/projectdata.h"
#include "Commands/CmdSchedule.h"

#include "localconfig.h"

#include "Mainwindow.h"

#include <QStyleHints>

WdgSchedule::WdgSchedule(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgSchedule),
    projectData(((MainWindow *)parent)->projectData()),
    _currentLine(nullptr),
    _currentLineDirection(nullptr),
    _scheduleStartTimeChanging(false) {
    ui->setupUi(this);

    // generate hour break actions
    for(int i = 0; i < 24; i++) {
        QAction *act = addAction(QString::number(i));
        act->setCheckable(true);
        connect(act, &QAction::triggered, this, [this, i]() {
            actionSetHourBreak(i);
        });
        _hourBreakActions << act;
    }

    ui->twSchedule->setContextMenuPolicy(Qt::ActionsContextMenu);

    _actionNew                       = ui->twSchedule->addAction(QIcon(":/icons/Add.ico"),              tr("Add Trip"));
    _actionCopy                      = ui->twSchedule->addAction(QIcon(":/icons/Duplicate.ico"),        tr("Copy Trip"));
    _actionOpenBusstopSchedule       = ui->twSchedule->addAction(QIcon(":/icons/BusstopSchedule.ico"),  tr("View Busstop Schedule"));
    _actionDelete                    = ui->twSchedule->addAction(QIcon(":/icons/Delete.ico"),           tr("Delete Trip(s)"));
    _actionShowOnlyImportantBusstops =                 addAction(QIcon(":/icons/ImportantBusstop.ico"), tr("Show only important Busstops"));
    _actionShowOnlyImportantBusstops->setCheckable(true);

    _actionDelete->setShortcut(Qt::Key_Delete);
    _actionDelete->setShortcutContext(Qt::WidgetShortcut);

    connect(ui->cbOnlyImportantBusstops, &QCheckBox::checkStateChanged, _actionShowOnlyImportantBusstops, [this](Qt::CheckState state){
        _actionShowOnlyImportantBusstops->setChecked(state == Qt::Checked);
    });

    connect(_actionShowOnlyImportantBusstops, &QAction::toggled, ui->cbOnlyImportantBusstops, &QCheckBox::setChecked);

    connect(ui->twSchedule,              &QTableWidget::itemSelectionChanged, this, &WdgSchedule::refreshReferences);
    connect(ui->twSchedule,              &QTableWidget::currentItemChanged,   this, &WdgSchedule::refreshReferences);

    connect(ui->twSchedule,              &QTableWidget::itemDoubleClicked,    this, &WdgSchedule::actionOpenBusstopSchedule);
    connect(_actionOpenBusstopSchedule,  &QAction::triggered,                 this, &WdgSchedule::actionOpenBusstopSchedule);

    connect(ui->cmbDirections,           &QComboBox::activated,               this, &WdgSchedule::actionChangeDirection);
    connect(ui->leSearchBusstop,         &QLineEdit::textChanged,             this, &WdgSchedule::refreshSchedule);
    connect(ui->cbOnlyImportantBusstops, &QCheckBox::stateChanged,            this, &WdgSchedule::refreshSchedule);


    ui->twSchedule->setRowHidden(0, true);

    refreshUI();
}

WdgSchedule::~WdgSchedule() {
    delete ui;
}

QList<QAction *> WdgSchedule::actions() {
    return ui->twSchedule->actions() << _actionShowOnlyImportantBusstops;
}

QList<QAction *> WdgSchedule::hourBreakActions() {
    return _hourBreakActions;
}

void WdgSchedule::refreshUI() {
    if(!_currentLine) {
        _actionNew                ->setEnabled(false);
        _actionCopy               ->setEnabled(false);
        _actionOpenBusstopSchedule->setEnabled(false);
        _actionDelete             ->setEnabled(false);
        return;
    }

    const int tripCount = _currentTrips.count();
    QTableWidgetItem *currentItem = ui->twSchedule->currentItem();

    _actionNew ->setEnabled(tripCount <= 1);
    _actionCopy->setEnabled(tripCount == 1);
    if(currentItem)
        _actionOpenBusstopSchedule->setEnabled(currentItem->row() >= headerRowCount);
    else
        _actionOpenBusstopSchedule->setEnabled(false);
    _actionDelete->setEnabled(tripCount > 0);
}

void WdgSchedule::refreshHourBreak() {
    for(int i = 0; i < 24; i++)
        _hourBreakActions[i]->setChecked(false);

    if(!_currentLine)
        return;

    _hourBreakActions[_currentLine->hourBreak()]->setChecked(true);
}

void WdgSchedule::refreshSchedule() {
    qDebug() << "refreshing schedule...";

    ui->twSchedule->blockSignals(true);

    const int currentScrollValue = ui->twSchedule->horizontalScrollBar()->value();

    ui->twSchedule->clearContents();
    ui->twSchedule->setColumnCount(0);
    ui->twSchedule->setRowCount(headerRowCount);

    _scheduleTableTripsReference.clear();

    if(!_currentLine) {
        ui->twSchedule->blockSignals(false);
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

    refreshBusstopList(trips);

    // get all repetitions and sort trips
    filteredTrips = ProjectData::sortTrips(filteredTrips, _currentLine->hourBreak());

    for(int i = 0; i < filteredTrips.count(); i++)
        refreshAddTrip(filteredTrips[i]);

    // resize columns and rows
    for(int i = 0; i <= ui->twSchedule->rowCount(); i++)
        ui->twSchedule->setRowHeight(i, 10);

    for(int i = 0; i <= ui->twSchedule->columnCount(); i++)
        ui->twSchedule->setColumnWidth(i, LocalConfig::timeFormat() == LocalConfig::Hours12 ? 60 : 50);

    if(ui->twSchedule->selectedItems().count() == 0) {
        _currentTrips = {};
    }

    ui->twSchedule->horizontalScrollBar()->setValue(currentScrollValue);

    ui->twSchedule->blockSignals(false);

    refreshUI();
}

void WdgSchedule::refreshBusstopList(QList<Trip *> trips) {
    _scheduleTableBusstopsReference.clear();

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
            for(int k = 0; k < _scheduleTableBusstopsReference.count(); k++) {
                if(_scheduleTableBusstopsReference[k] == b) {
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
                targetIndex = _scheduleTableBusstopsReference.count();
            } else {
                targetIndex = 0;
            }

            for(int k = 0; k < _scheduleTableBusstopsReference.count(); k++) {
                if(_scheduleTableBusstopsReference[k] == lastBusstop) {
                    targetIndex = k + 1;
                    break;
                }
            }

            ui->twSchedule->insertRow(targetIndex + headerRowCount);
            ui->twSchedule->setVerticalHeaderItem(targetIndex + headerRowCount, new QTableWidgetItem(b->name()));
            _scheduleTableBusstopsReference.insert(targetIndex, b);
            lastBusstop = b;
        }
    }
}

void WdgSchedule::refreshAddTrip(Trip *t) {
    if(!t)
        return;

    Trip *parentT = projectData->trip(t->id());
    if(parentT)
        _scheduleTableTripsReference << parentT;

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
    QTableWidgetItem *itmStartTime   = new QTableWidgetItem(t->startTime().toString(LocalConfig::timeFormatString(false, false)));

    //------------------------------

    QList<Tour *> tours = ProjectData::sortItems(projectData->toursOfTrip(t));

    if(tours.isEmpty()) {
        tour = tr("None");
    } else {
        QStringList strListTours;
        tourToolTip = "<ul style=\"margin: 0; padding: 0\">";
        for(int i = 0; i < tours.count(); i++) {
            strListTours << tours[i]->name();
            tourToolTip += "<li><b>" + tours[i]->name() + "</b> (" + tours[i]->weekDays().toString() + ")</li>";
        }
        tourToolTip += "</ul>";
        tour = strListTours.join(", ");
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
            itmTour->setBackground(QColor(255, 168, 0));
            itmTour->setForeground(Qt::black);
            tourToolTip = tr("<p><b style=\"color: red;\">Multiple assignment at %1:</b><p>%2").arg(overlapStr, tourToolTip);
        } else {
            itmTour->setBackground(Qt::blue);
            itmTour->setForeground(Qt::white);
        }
    }

    itmTour->setText(tour);
    itmTour->setToolTip(!tours.isEmpty() ? "<html><body>" + tourToolTip + "</body></html>" : "");

    //------------------------------

    QList<Footnote *> footnotes = projectData->autoAssignedFootnotesOfTrip(t);
    QStringList footnotesStrList;
    QString footnotesToolTipStr = "<html><body><table>";
    for(int i = 0; i < footnotes.count(); i++) {
        footnotesStrList << footnotes[i]->identifier();
        footnotesToolTipStr += "<tr><td><b>" + footnotes[i]->identifier() + ": </b></td><td>" + footnotes[i]->description() + "</td></tr>";
    }

    itmFootnotes->setText(footnotesStrList.join(", "));
    itmFootnotes->setToolTip(!footnotes.isEmpty() ? footnotesToolTipStr + "</table></body></html>" : "");

    if(!footnotesStrList.isEmpty()) {
        itmFootnotes->setBackground(Qt::yellow);
        itmFootnotes->setForeground(Qt::black);
    }

    //------------------------------

    itmRoute->setToolTip(itmRoute->text());

    //------------------------------

    int timeProfileIndex = t->route()->indexOfTimeProfile(t->timeProfile());
    QList<QColor> profileColors = {QColor(0, 163, 255), QColor(185, 0, 255), QColor(255, 0, 184), QColor(255, 134, 0), QColor(128, 255, 0)};
    QColor currentProfileColor = (timeProfileIndex >= 4 || timeProfileIndex < 0) ? profileColors[4] : profileColors[timeProfileIndex];
    itmTimeProfile->setBackground(currentProfileColor);
    itmTimeProfile->setForeground(global::getContrastColor(currentProfileColor));
    itmTimeProfile->setToolTip(itmTimeProfile->text());


    //------------------------------

    itmStartTime->setFont(bold);
    itmStartTime->setBackground(QColor(85, 85, 85));
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
        Busstop *b = _scheduleTableBusstopsReference[i - headerRowCount];
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
        QString timeStr = t->startTime().addSecs(depVal * 60).toString(LocalConfig::timeFormatString(false, true));
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
        if((i % 2) == 0 && i >= headerRowCount) { // "zebrastreifen"-look
            if(qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
                itm->setBackground(QColor(31, 31, 31));
            else
                itm->setBackground(QColor(224, 224, 224));
        }
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

void WdgSchedule::refreshDirections() {
    qDebug() << "refreshing line directions in schedule";

    int lastIndex = ui->cmbDirections->currentIndex();

    ui->cmbDirections->clear();
    _lineDirectionsReference.clear();

    if(!_currentLine)
        return;

    for(int i = 0; i < _currentLine->directionCount(); i++) {
        LineDirection *ld = _currentLine->directionAt(i);
        _lineDirectionsReference << ld;
        ui->cmbDirections->addItem(ld->description());
    }

    if(_lineDirectionsReference.count() > lastIndex)
        ui->cmbDirections->setCurrentIndex(lastIndex);
    else
        ui->cmbDirections->setCurrentIndex(ui->cmbDirections->count() - 1);

    if(lastIndex == -1)
        ui->cmbDirections->setCurrentIndex(0);

    int currentIndex = ui->cmbDirections->currentIndex();

    if(!_lineDirectionsReference.empty())
        _currentLineDirection = _lineDirectionsReference[currentIndex];
    else
        _currentLineDirection = nullptr;
}

void WdgSchedule::refreshDayTypes() {
    qDebug() << "refreshing day types in schedule";
    _dayTypesReference = projectData->projectSettings()->dayTypes();

    ui->cmbDayTypes->clear();
    for(int i = 0; i < _dayTypesReference.count(); i++)
        ui->cmbDayTypes->addItem(_dayTypesReference[i]->name());

    if(!_dayTypesReference.empty()) {
        ui->cmbDayTypes->setCurrentIndex(0);
        _currentDayType = _dayTypesReference[0];
    }
}

void WdgSchedule::setCurrentLine(Line *l) {
    _currentLine = l;
    _currentTrips.clear();
    refreshDirections();
    refreshSchedule();
    refreshHourBreak();
    emit currentLineChanged(_currentLine, _currentLineDirection);
    emit currentTripsChanged(_currentTrips);
    refreshUI();
}

void WdgSchedule::setCurrenTrips(const QList<Trip *> trips) {
    _currentTrips = trips;
    refreshUI();
}

void WdgSchedule::actionChangeDirection()
{
    _currentLineDirection = _lineDirectionsReference[ui->cmbDirections->currentIndex()];
    _currentTrips.clear();
    refreshSchedule();
    emit currentLineChanged(_currentLine, _currentLineDirection);
    emit currentTripsChanged(_currentTrips);
}

void WdgSchedule::actionSetHourBreak(const int &newLineHourBreak) {
    if(!_currentLine)
        return;

    for(int i = 0; i < 24; i++)
        _hourBreakActions[i]->setChecked(false);
    _hourBreakActions[newLineHourBreak]->setChecked(true);

    projectData->undoStack()->push(new CmdScheduleChangeHourBreak(_currentLine, newLineHourBreak));
    refreshSchedule();
}

void WdgSchedule::actionOpenBusstopSchedule() {
    QTableWidgetItem *item = ui->twSchedule->currentItem();
    if(!item)
        return;

    if(item->row() == 1) {
        int index = item->column();
        Trip *t = _scheduleTableTripsReference[index];
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
        if(index < 0 || index >= _scheduleTableBusstopsReference.count())
            return;

        Busstop *b = _scheduleTableBusstopsReference[index];
        QList<Route *> routes;
        for (int i = 0; i < _currentLine->routeCount(); ++i) {
            Route *r = _currentLine->routeAt(i);
            if(r->direction() == _currentLineDirection)
                routes << r;
        }
        emit busstopScheduleRequested(b, routes, _currentDayType);
    }
}

bool WdgSchedule::checkMatchingWeekdays(WeekDays d) {
    if(!_currentDayType)
        return false;

    return WeekDays::overlap(d, *_currentDayType);
}

void WdgSchedule::on_cmbDayTypes_activated(int index) {
    _currentDayType = _dayTypesReference[index];
    _currentTrips.clear();
    refreshSchedule();
    emit currentDayTypeChanged(*_currentDayType);
    emit currentTripsChanged(_currentTrips);
}

void WdgSchedule::refreshReferences() {
    _currentTrips.clear();

    for(int i = 0; i < _scheduleTableTripsReference.count(); i++) {
        QTableWidgetItem *itm = ui->twSchedule->item(0, i);
        if(itm->isSelected())
            _currentTrips << _scheduleTableTripsReference[i];
    }

    refreshUI();
    emit currentTripsChanged(_currentTrips);
}
