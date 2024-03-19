#include "Dialogs/DlgTripselector.h"
#include "ui_DlgTripselector.h"


#include "App/global.h"
#include "ProjectData\busstop.h"

TripSelector::TripSelector(QWidget *parent, ProjectData *p, Trip *t, WeekDays d) :
    QDialog(parent),
    ui(new Ui::TripSelector),
    projectData(p),
    previousTrip(t),
    weekDays(d),
    currentDirection(nullptr)
{
    ui->setupUi(this);

    if(previousTrip) {
        startBusstop = previousTrip->route()->lastBusstop();
        startTime = previousTrip->endTime();
    } else {
        startBusstop = nullptr;
        startTime = QTime(0, 0, 0, 0);
    }

    ui->twLines->setColumnHidden(0, true);
    ui->twLines->setColumnWidth(1, 50);

    ui->twTrips->setColumnHidden(0, true);
    ui->twTrips->setColumnWidth(1, 50);
    ui->twTrips->setColumnWidth(2, 50);
    ui->twTrips->setColumnWidth(3, 200);
    ui->twTrips->setColumnWidth(4, 200);

    loadLines();
}

TripSelector::~TripSelector()
{
    delete ui;
}

void TripSelector::loadLines()
{
    Line *prevLine = projectData->lineOfTrip(previousTrip);
    
    for(int i = 0; i < projectData->lineCount(); i++) {
        Line *l = projectData->lineAt(i);
        QTreeWidgetItem *itm = new QTreeWidgetItem(ui->twLines);
        itm->setText(0, l->id());
        itm->setText(1, l->name());
        itm->setText(2, l->description());
        
        QColor color = l->color();
        QColor bColor = global::getContrastColor(color);
        itm->setBackground(1, color);
        itm->setForeground(1, bColor);


        if(!prevLine)
            continue;

        if(l->id() == prevLine->id())
            ui->twLines->setCurrentItem(itm);
    }
}

void TripSelector::loadTrips()
{
    QFont bold;
    bold.setBold(true);

    QFont italic;
    italic.setItalic(true);

    ui->twTrips->clear();
    QTreeWidgetItem *itm = ui->twLines->currentItem();
    if(!itm)
        return;

    QString lineID = itm->text(0);
    Line *l = projectData->line(lineID);

    if(!l)
        return;

    QList<Trip *> trips;
    trips = l->tripsToDirection(currentDirection);

    QList<Trip *> resultList;

    for(int i = 0; i < trips.count(); i++) {
        Trip *t = trips[i];
        
        if(!weekDays.isIn(t->weekDays()))
            continue;

        resultList << t;
    }
    resultList = ProjectData::sortItems(resultList);

    QList<Trip *> resultListAdd;
    if(startTime >= QTime(12, 0, 0, 0)) {
        for(int i = 0; i < trips.count(); i++) {
            Trip *t = trips[i];

            if(t->startTime() >= QTime(12, 0, 0, 0))
                continue;

            if(!weekDays.shfitedToNextDay().isIn(t->weekDays()))
                continue;

            resultListAdd << t;
        }
    }
    
    resultListAdd = ProjectData::sortItems(resultListAdd);

    resultList << resultListAdd;

    for(int i = 0; i < resultList.count(); i++) {
        Trip *t = resultList[i];
        QTreeWidgetItem *itm = new QTreeWidgetItem(ui->twTrips);
        int breakTime = (t->startTime().msecsSinceStartOfDay() - startTime.msecsSinceStartOfDay()) / (1000 * 60);
        if(breakTime < 0)
            breakTime += 1440;

        itm->setText(0, t->id());
        itm->setText(1, t->startTime().toString("hh:mm"));
        itm->setText(2, QString::number(breakTime) + tr(" min."));
        itm->setText(3, t->route()->firstBusstop()->name());
        itm->setText(4, t->route()->lastBusstop()->name());
        itm->setToolTip(3, t->route()->name());
        itm->setToolTip(4, t->route()->name());
        
        if((startBusstop && t->route()->firstBusstop()->id() != startBusstop->id()) || t->startTime() < startTime) {
            itm->setFont(1, italic);
            itm->setFont(2, italic);
            itm->setFont(3, italic);
            itm->setFont(4, italic);
            itm->setForeground(1, Qt::gray);
            itm->setForeground(2, Qt::gray);
            itm->setForeground(3, Qt::gray);
            itm->setForeground(4, Qt::gray);
        } else {
            itm->setFont(1, bold);
            itm->setFont(2, bold);
            itm->setFont(3, bold);
            itm->setFont(4, bold);
        }
        
        if(startBusstop && t->route()->firstBusstop()->id() != startBusstop->id()) {
            itm->setForeground(3, Qt::red);
        }
        
        if(t->startTime() >= startTime) {
            itm->setForeground(1, Qt::darkGreen);
            itm->setForeground(2, Qt::darkGreen);
            if(!projectData->toursOfTrip(t).empty()) {
                itm->setForeground(3, QColor(255, 0, 255));
            }
        }
    }

    ui->twTrips->resizeColumnToContents(2);
    ui->twTrips->resizeColumnToContents(3);
}


Trip * TripSelector::getTrip()
{
    QTreeWidgetItem *itm = ui->twTrips->currentItem();
    if(!itm)
        return nullptr;

    QString tripID = itm->text(0);
    for(int i = 0; i < projectData->lineCount(); i++) {
        Line *l = projectData->lineAt(i);
        Trip *t = l->trip(tripID);
        if(!t)
            continue;

        return t;
    }

    return nullptr;
}

void TripSelector::on_twLines_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous);

    QString lineID = current->text(0);
    Line *l = projectData->line(lineID);

    if(!l)
        return;

    // refresh directions
    int lastIndex = ui->cmbDirections->currentIndex();

    ui->cmbDirections->clear();
    directionsReference.clear();

    for(int i = 0; i < l->directionCount(); i++) {
        LineDirection *ld = l->directionAt(i);
        ui->cmbDirections->addItem(ld->description());
        directionsReference << ld;
        if(i == lastIndex) {
            ui->cmbDirections->setCurrentIndex(i);
            currentDirection = ld;
        }
    }

    loadTrips();
}


void TripSelector::on_twTrips_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QDialog::accept();
}


void TripSelector::on_cmbDirections_activated(int index) {
    currentDirection = directionsReference[index];
    loadTrips();
}

