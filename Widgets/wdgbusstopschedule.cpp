#include "Widgets/wdgbusstopschedule.h"
#include "qevent.h"
#include "ui_wdgbusstopschedule.h"
#include "App/global.h"

#include <QTableWidgetItem>
#include <QMessageBox>
#include <QEvent>

WdgBusstopSchedule::WdgBusstopSchedule(QWidget *parent, ProjectData *projectData) :
    QWidget(parent),
    ui(new Ui::WdgBusstopSchedule),
    projectData(projectData)
{
    ui->setupUi(this);

    ui->twSchedule->horizontalHeader()->setVisible(false);

    ui->progressBar->setHidden(true);
    ui->lLoading->setHidden(true);

    QFont bold;
    bold.setBold(true);
    ui->lBusstopName->setFont(bold);

    ui->routeSelector->setProjectData(projectData);

    QObject::connect(ui->routeSelector, SIGNAL(routesChanged(QList<Route*>)), this, SLOT(refreshSchedule()));
}

WdgBusstopSchedule::~WdgBusstopSchedule() {
    delete ui;
}

void WdgBusstopSchedule::refreshSchedule() {
    ui->cmbDayTypes->clear();

    for(int i = 0; i < projectData->projectSettings()->dayTypeCount(); i++) {
        DayType *dt = projectData->projectSettings()->dayTypeAt(i);
        ui->cmbDayTypes->addItem(dt->name());
        if(dt == currentDayType)
            ui->cmbDayTypes->setCurrentIndex(i);
    }

    ui->twSchedule->clear();
    ui->twSchedule->setRowCount(0);
    ui->twSchedule->setColumnCount(0);

    for(int i = 0; i < 24; i++) {
        ui->twSchedule->insertRow(i);
        ui->twSchedule->setVerticalHeaderItem(i, new QTableWidgetItem(QString::number(i)));
    }

    if(!busstop)
        return;

    refreshing = true;

    QList<Route *> routes = ui->routeSelector->routes();
    QList<Trip *> trips;

    foreach(Line *l, projectData->lines()) {
        foreach(Trip *t, l->trips()) {
            bool matchingRoute = false;
            foreach(Route *r, routes) {
                if(t->route() == r) {
                    matchingRoute = true;
                    break;
                }
            }

            if(!matchingRoute)
                continue;

            WeekDays w = getShiftedWeekDays(t);
            if(checkMatchingWeekDays(w))
                trips << t;
        }
    }

    std::sort(trips.begin(), trips.end(), [this](Trip *a, Trip *b) {return a->busstopTime(this->busstop) < b->busstopTime(this->busstop);});

    int hour = 0;
    int column = 0;
    for(int i = 0; i < trips.count(); i++) {
        Trip *t = trips[i];
        QTime time = t->busstopTime(busstop);

        if(time.hour() != hour) {
            hour = time.hour();
            column = 0;
        }

        if(ui->twSchedule->columnCount() <= column) {
            ui->twSchedule->insertColumn(column);
            ui->twSchedule->setColumnWidth(column, 25);
        }
        
        Line *l = projectData->lineOfTrip(t);
        QTableWidgetItem *itm = new QTableWidgetItem(QString::number(time.minute()));
        QFont f;
        f.setPixelSize(20);
        itm->setFont(f);
        itm->setToolTip(t->route()->name() + " (" + getShiftedWeekDays(t).toString() + ")");
        if(l) {
            itm->setBackground(l->color());
            itm->setForeground(global::getContrastColor(l->color()));
        }

        ui->twSchedule->setItem(hour, column, itm);
        column++;
    }

    ui->progressBar->setHidden(true);

    ui->progressBar->setValue(0);
    ui->lLoading->setHidden(true);
    refreshing = false;
}

WeekDays WdgBusstopSchedule::getShiftedWeekDays(Trip *t) {
    WeekDays w(nullptr);
    if(!t->goesPastMidnight())
        w = t->weekDays();
    else
        if(t->busstopIsAfterMidnight(busstop))
            w = t->weekDays().shfitedToNextDay();
        else
            w = t->weekDays();
    return w;
}

bool WdgBusstopSchedule::checkMatchingWeekDays(WeekDays w) {
    if(!currentDayType)
        return false;

    return WeekDays::overlap(w, WeekDays(*currentDayType));
}

void WdgBusstopSchedule::showEvent(QShowEvent *e) {
    QWidget::showEvent(e);
    refreshSchedule();
}

void WdgBusstopSchedule::setBusstop(Busstop *b) {
    busstop = b;
    if(!b)
        ui->lBusstopName->setText(tr("<unkown>"));
    else
        ui->lBusstopName->setText(b->name());

    ui->routeSelector->setFilterBusstop(b);
    ui->routeSelector->refresh();
    ui->routeSelector->expandAll();

    refreshSchedule();
}

void WdgBusstopSchedule::setRoutes(QList<Route *> routes) {
    ui->routeSelector->setSelectedRoutes(routes);
}

void WdgBusstopSchedule::setAll(Busstop *b, QList<Route *> routes, DayType *days) {
    if(refreshing)
        return;

    setBusstop(b);
    setRoutes(routes);
    currentDayType = days;

    if(this->isVisible())
        refreshSchedule();
}

void WdgBusstopSchedule::on_cmbDayTypes_activated(int index) {
    currentDayType = projectData->projectSettings()->dayTypeAt(index);
    refreshSchedule();
}

