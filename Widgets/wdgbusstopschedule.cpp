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

    int dayTypeIndex = ui->cmbDayTypes->currentIndex();
    int dayTypeCount = projectData->projectSettings()->dayTypeCount();
    if(dayTypeCount < 0 || dayTypeCount <= dayTypeIndex)
        return;

    currentDayType = projectData->projectSettings()->dayTypeAt(dayTypeIndex);

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
        }

        QList<Footnote *> footnotes = ProjectData::sortItems(projectData->autoAssignedFootnotesOfTrip(t));
        QStringList footnotesStr;
        QStringList footnotesToolTip;
        for(int i = 0; i < footnotes.count(); i++) {
            footnotesStr << "<b>" + footnotes[i]->identifier() + "</b>";
            footnotesToolTip << "<tr><td><b>" + footnotes[i]->identifier() + ": </b></td><td>" + footnotes[i]->description() + "</td></tr>";
        }
        
        Line *l = projectData->lineOfTrip(t);
        QTableWidgetItem *itm = new QTableWidgetItem;

        itm->setToolTip("<html><body><b>" + t->route()->name() + "</b><br>(" + getShiftedWeekDays(t).toString() + ")" + (footnotes.isEmpty() ? "" : "<hr><table>" + footnotesToolTip.join("") + "</table></body></html>"));
        ui->twSchedule->setItem(hour, column, itm);
        QLabel *label = new QLabel(QString::number(time.minute()) + "<sub>" + footnotesStr.join(",") + "</sub>");
        label->setMargin(3);

        if(l) {
            itm->setBackground(l->color());
            label->setStyleSheet("color: " + global::getContrastColor(l->color()).name());
        }
        QFont f;
        f.setPixelSize(20);
        label->setFont(f);

        ui->twSchedule->setCellWidget(hour, column, label);
        column++;
    }

    ui->twSchedule->resizeColumnsToContents();
    int maxColumnWidth = 0;
    for(int i = 0; i < ui->twSchedule->columnCount(); i++)
        maxColumnWidth = std::max(maxColumnWidth, ui->twSchedule->columnWidth(i));

    for(int i = 0; i < ui->twSchedule->columnCount(); i++)
        ui->twSchedule->setColumnWidth(i, maxColumnWidth);

    ui->twSchedule->resizeRowsToContents();
    int maxRowHeight = 0;
    for(int i = 0; i < ui->twSchedule->rowCount(); i++)
        maxRowHeight = std::max(maxRowHeight, ui->twSchedule->rowHeight(i));

    for(int i = 0; i < ui->twSchedule->rowCount(); i++)
        ui->twSchedule->setRowHeight(i, maxRowHeight);

    refreshing = false;
}

WeekDays WdgBusstopSchedule::getShiftedWeekDays(Trip *t) {
    WeekDays w;
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

