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

    ui->twLines->setColumnHidden(0, true);
    ui->twLines->setHeaderHidden(true);

    ui->twDirections->setColumnHidden(0, true);
    ui->twDirections->setHeaderHidden(true);

    ui->twSchedule->horizontalHeader()->setVisible(false);

    ui->progressBar->setHidden(true);
    ui->lLoading->setHidden(true);

    QFont bold;
    bold.setBold(true);
    ui->lBusstopName->setFont(bold);
}

WdgBusstopSchedule::~WdgBusstopSchedule()
{
    delete ui;
}

void WdgBusstopSchedule::on_twDirections_itemChanged() { refreshSchedule(); }


void WdgBusstopSchedule::on_twLines_itemChanged() { refreshSchedule(); }

void WdgBusstopSchedule::refreshSchedule()
{
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

    QList<Trip *> mainTrips;

    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(ui->twLines->topLevelItemCount());
    //ui->progressBar->setHidden(false);
    ui->lLoading->setHidden(false);
    qApp->processEvents();

    for(int i = 0; i < ui->twLines->topLevelItemCount(); i++) { // alle selektierten Linien durchlaufen

        if(ui->twLines->topLevelItem(i)->checkState(1) != Qt::Checked)
            continue;
        
        Line *l = projectData->line(ui->twLines->topLevelItem(i)->text(0));
        if(!l)
            continue;

        int count = l->tripCount();
        QList<Trip *> trips = l->trips();

        for(int j = 0; j < count; j++) { // alle Trips durchlaufen
            Trip *t = trips[j];
            Route *r = t->route();

            for(int k = 0; k < r->busstopCount(); k++) { // Bushaltestellen der Route des Trips durchlaufen
                Busstop *cBusstop = r->busstopAt(k);
                Busstop *dirBusstop;
                if(cBusstop == busstop) { // wenn die busstop ids identisch sind, ...
                    if(r->busstopCount() > k + 1) {
                        dirBusstop = r->busstopAt(k + 1); // setze die ID der Fahrtrichtung auf die ID der nächsten Haltestelle
                    } else {
                        continue;
                    }

                    for(int n = 0; n < ui->twDirections->topLevelItemCount(); n++) { // alle Richtungen durchlaufen
                        if(ui->twDirections->topLevelItem(n)->text(0) == dirBusstop->id()) { // wenn die IDs übereinstimmen und das ganze angehakt ist, hinzufügen
                            if(ui->twDirections->topLevelItem(n)->checkState(1) == Qt::Checked) {
                                WeekDays w = getShiftedWeekDays(t);
                                if(checkMatchingWeekDays(w))
                                    mainTrips << t;
                            }
                        }
                    }
                }
                qApp->processEvents();
            }
            qApp->processEvents();
        }

        ui->progressBar->setValue(i + 1);
        qApp->processEvents();
    }

    QList<Trip *> allTrips;

    for(int i = 0; i < mainTrips.count(); i++) {
        if(mainTrips[i]->hasRepeat())
            for(int j = 0; j < mainTrips[i]->childCount(); j++)
                allTrips << mainTrips[i]->childAt(j);
        else
            allTrips << mainTrips[i];
    }

    // sort
    ui->progressBar->setMaximum(0);
    while(true) {
        bool ok = true;
        for(int i = 0; i < allTrips.count() - 1; i++) {
            if(allTrips[i]->busstopTime(busstop) > allTrips[i + 1]->busstopTime(busstop)) {
                Trip *tmp = allTrips[i];
                allTrips[i] = allTrips[i + 1];
                allTrips[i + 1] = tmp;
                ok = false;
            }
            qApp->processEvents();
        }

        qApp->processEvents();

        if(ok)
            break;
    }

    int hour = 0;
    int column = 0;
    for(int i = 0; i < allTrips.count(); i++) {
        Trip *t = allTrips[i];
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
            itm->setBackground(l->getColor());
            itm->setForeground(global::getContrastColor(l->getColor()));
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
    WeekDays w;
    if(!t->goesPastMidnight())
        w = *t->weekDays();
    else
        if(t->busstopIsAfterMidnight(busstop))
        w = t->weekDays()->shfitedToNextDay();
        else
        w = *t->weekDays();
    return w;
}

bool WdgBusstopSchedule::checkMatchingWeekDays(WeekDays w)
{
    if(ui->rbMonFri->isChecked())
        return (
            w.monday() ||
            w.tuesday() ||
            w.wednesday() ||
            w.thursday() ||
            w.friday()
        );

    else if(ui->rbSat->isChecked())
        return w.saturday();

    else if(ui->rbSun->isChecked())
        return (w.sunday() || w.holiday());

    return false;
}


void WdgBusstopSchedule::on_rbMonFri_clicked()
{
    refreshSchedule();
}


void WdgBusstopSchedule::on_rbSat_clicked()
{
    refreshSchedule();
}


void WdgBusstopSchedule::on_rbSun_clicked()
{
    refreshSchedule();
}

void WdgBusstopSchedule::showEvent(QShowEvent *e) {
    QWidget::showEvent(e);
    refreshLinesAndDirections();
    refreshSchedule();
}

void WdgBusstopSchedule::setBusstop(Busstop *b) {
    busstop = b;
    if(!b)
        ui->lBusstopName->setText(tr("<unkown>"));
    else
        ui->lBusstopName->setText(b->name());
}

void WdgBusstopSchedule::setDirections(QList<Busstop *> list) {
    if(!refreshing)
        preselectedDirections = list;
}

void WdgBusstopSchedule::setLines(QList<Line *> list) {
    if(!refreshing)
        preselectedLines = list;
}

void WdgBusstopSchedule::setDays(int i) {
    if(refreshing)
        return;

    if(i == 1)
        ui->rbMonFri->setChecked(true);
    else if(i == 2)
        ui->rbSat->setChecked(true);
    else if(i == 3)
        ui->rbSun->setChecked(true);
}

void WdgBusstopSchedule::setAll(Busstop *b, QList<Busstop *> directions, QList<Line *> lines, int days) {
    if(refreshing)
        return;

    setDirections(directions);
    setLines(lines);
    setDays(days);
    setBusstop(b);

    if(this->isVisible()) {
        refreshLinesAndDirections();
        refreshSchedule();
    }
}

void WdgBusstopSchedule::refreshLinesAndDirections() {

    ui->twDirections->clear();
    ui->twLines->clear();
    
    for(int i = 0; i < projectData->lineCount(); i++) {
        Line *l = projectData->lineAt(i);
        for(int j = 0; j < l->routeCount(); j++) {
            Route *r = l->routeAt(j);
            for(int k = 0; k < r->busstopCount(); k++) {
                Busstop *c = r->busstopAt(k);
                if(c == busstop) {
                    QTreeWidgetItem *itm = new QTreeWidgetItem(ui->twLines);
                    itm->setText(0, l->id());
                    itm->setText(1, l->name());
                    itm->setBackground(1, l->getColor());
                    itm->setForeground(1, global::getContrastColor(l->getColor()));

                    if(preselectedLines.empty())
                        itm->setCheckState(1, Qt::Checked);
                    else
                        if(preselectedLines.contains(l))
                            itm->setCheckState(1, Qt::Checked);
                        else
                            itm->setCheckState(1, Qt::Unchecked);

                    ui->twLines->addTopLevelItem(itm);
                    goto nextLine;
                }
            }
        }
    nextLine:
        true;
    }

    QList<Busstop *> directions;
    
    for(int i = 0; i < projectData->lineCount(); i++) {
        Line *l = projectData->lineAt(i);
        for(int j = 0; j < l->routeCount(); j++) {
            Route *r = l->routeAt(j);
            for(int k = 0; k < r->busstopCount(); k++) {
                Busstop *c = r->busstopAt(k);
                if(c == busstop) {
                    if(r->busstopCount() > k + 1)
                            directions << r->busstopAt(k + 1);
                }
            }
        }
    }

    QList<Busstop *> directionsFiltered;
    for(int i = 0; i < directions.count(); i++) {
        Busstop *c = directions[i];

        bool found = false;
        for(int j = 0; j < directionsFiltered.count(); j++)
            if(directionsFiltered[j]->id() == c->id())
                found = true;

        if(found)
            continue;

        directionsFiltered << c;

        QTreeWidgetItem *itm = new QTreeWidgetItem(ui->twDirections);
        itm->setText(0, c->id());
        itm->setText(1, c->name());
        if(preselectedDirections.contains(c))
            itm->setCheckState(1, Qt::Checked);
        else
            itm->setCheckState(1, Qt::Unchecked);
        ui->twDirections->addTopLevelItem(itm);
    }
}

