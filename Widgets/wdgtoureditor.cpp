#include "WdgTourEditor.h"
#include "ui_wdgtoureditor.h"

#include <QMessageBox>
#include <QScrollBar>
#include <QInputDialog>

#include "Dialogs/DlgTripselector.h"
#include "Dialogs/DlgDataexporter.h"
#include "Commands/cmdtours.h"

#include "App/global.h"

WdgTourEditor::WdgTourEditor(QWidget *parent, ProjectData *projectData, QUndoStack *undoStack) :
    QWidget(parent),
    ui(new Ui::WdgTourEditor),
    projectData(projectData),
    undoStack(undoStack)
{
    ui->setupUi(this);

    QObject::connect(ui->cbDifferentHeights, SIGNAL(stateChanged(int)), this, SLOT(actionTourChangeProportionalRowHeight()));
    QObject::connect(ui->cbShowBreaks, SIGNAL(stateChanged(int)), this, SLOT(actionTourChangeShowBreaks()));
    QObject::connect(ui->hsTourScale, SIGNAL(valueChanged(int)), this, SLOT(actionTourChangeScale()));
    QObject::connect(ui->pbTripNew, SIGNAL(clicked()), this, SLOT(actionTourAddTrip()));
    QObject::connect(ui->twNextTrips, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(actionTourAddTrip()));
    QObject::connect(ui->pbTripDelete, SIGNAL(clicked()), this, SLOT(actionTourRemoveTrip()));
    QObject::connect(ui->pbTripsMore, SIGNAL(clicked()), this, SLOT(actionTourAddMoreTrips()));
    QObject::connect(ui->pbReorderTrips, SIGNAL(clicked()), this, SLOT(actionReorderTrips()));
    QObject::connect(ui->pbExport, SIGNAL(clicked()), this, SLOT(actionExport()));

    ui->twTour->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->twTour->verticalHeader()->setVisible(false);
    ui->twTour->setColumnWidth(0, 50);
    ui->twTour->setColumnWidth(1, 50);
    ui->twTour->setColumnWidth(2, 100);
    ui->twTour->setColumnWidth(3, 400);

    ui->twNextTrips->setColumnHidden(0, true);
    ui->twNextTrips->setColumnWidth(1, 50);
    ui->twNextTrips->setColumnWidth(2, 75);
}

WdgTourEditor::~WdgTourEditor()
{
    delete ui;
}



void WdgTourEditor::actionTourChangeProportionalRowHeight() {
    refreshTour();
}

void WdgTourEditor::actionTourChangeShowBreaks() {
    refreshTour();
}

void WdgTourEditor::actionTourChangeScale() {
    refreshTour();
}

void WdgTourEditor::actionTourAddTrip() {
    if(!m_currentTour || !m_currentNextTrip)
        return;

    int index;
    if(!m_currentTrip)
        index = m_currentTour->tripCount();
    else
        index = m_currentTour->indexOfTrip(m_currentTrip) + 1;

    undoStack->push(new cmdTourTripAdd(m_currentTour, m_currentNextTrip, index));
    m_currentTrip = m_currentNextTrip;
    refreshTour();
}

void WdgTourEditor::actionTourRemoveTrip() {
    if(!m_currentTour || !m_currentTrip)
        return;

    QMessageBox::StandardButton msg = QMessageBox::warning(this, "remove trip from tour", "<p><b>Do you really want to remove this trip from this tour?</b><p><p>It will not be deleted from schedule.</p>", QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;
    
    undoStack->push(new cmdTourTripRemove(m_currentTour, m_currentTrip, m_currentTour->indexOfTrip(m_currentTrip)));
    refreshTour();
}

void WdgTourEditor::actionTourAddMoreTrips() {
    /*Trip *lt = currentTrip();
    Tour *o = m_currentTrip;*/
    if(!m_currentTour)
        return;
    
    TripSelector dlg(this, projectData, m_currentTrip, m_currentTour->weekDays());
    dlg.exec();
    Trip *t = dlg.getTrip();
    if(!t)
        return;

    int index;
    if(!m_currentTrip)
        index = m_currentTour->tripCount();
    else
        index = m_currentTour->indexOfTrip(m_currentTrip) + 1;

    undoStack->push(new cmdTourTripAdd(m_currentTour, t, index));
    m_currentTrip = t;
    refreshTour();
}

void WdgTourEditor::actionReorderTrips() {
    if(!m_currentTour)
        return;

    undoStack->push(new cmdTourReorderTrips(m_currentTour));
    refreshTour();
}

void WdgTourEditor::actionExport() {
    if(!m_currentTour)
        return;

    bool ok = false;
    QString aiGroupName = QInputDialog::getText(this, "AI-Group", "Enter target AI-Group name", QLineEdit::Normal, "GVB - Gelenk", &ok);
    if(!ok)
        return;

    QString result;

    result = "[newtour]\r\n" + m_currentTour->name() + "\r\n" + aiGroupName + "\r\n799\r\n\r\n";

    QList<Trip *> trips = m_currentTour->trips();
    for(int i = 0; i < trips.count(); i++) {
        Trip *t = trips[i];
        int timeProfileIndex = 0;
        int time = t->startTime().msecsSinceStartOfDay() / 1000 / 60;
        //float time = static_cast<int>(minutes / 60) + (minutes % 60);
        //qDebug() << minutes << time;
        Route *r = t->route();
        for(int j = 0; j < r->timeProfileCount(); j++) {
            if(r->timeProfileAt(j) == t->timeProfile()) {
                timeProfileIndex = j;
                break;
            }
        }
        result += "  Dep.: " + t->startTime().toString("hh:mm:ss") + "\r\n[addtrip]\r\n" + t->route()->name() + "\r\n" + QString::number(timeProfileIndex) + "\r\n" + QString::number(time) + "\r\n\r\n";
    }

    DataExporter dlg;
    dlg.setFileName(m_currentTour->name());
    dlg.setFolderName("tours");
    dlg.setText(result, "", "");
    dlg.exec();
}



Trip * WdgTourEditor::currentTrip()
{
    QTableWidgetItem *itm = ui->twTour->currentItem();
    if(!itm)
        return nullptr;

    QTableWidgetItem *tItm = ui->twTour->item(itm->row(), 0);
    if(!tItm)
        return nullptr;
    
    for(int i = 0; i < projectData->lineCount(); i++) {
        Line *l = projectData->lineAt(i);
        Trip *t = l->trip(tItm->text());
        if(t)
            return t;
    }

    return nullptr;
}

void WdgTourEditor::refreshTour() {
    Trip *lastCurrentTrip = m_currentTrip;
    QFont bold;
    bold.setBold(true);

    QFont italic;
    italic.setItalic(true);

    int scrollValue = ui->twTour->verticalScrollBar()->value();

    ui->twTour->clearContents();
    ui->twTour->setRowCount(0);

    tourTableReference.clear();

    if(!m_currentTour)
        return;

    Trip *lastTrip = nullptr;
    QStringList lines;

    for(int i = 0; i < m_currentTour->tripCount(); i++) {
        int targetRow = ui->twTour->rowCount();

        Trip *t = m_currentTour->tripAt(i);

        int breakDuration = 0;
        if(lastTrip)
            breakDuration = (t->startTime().msecsSinceStartOfDay() - lastTrip->endTime().msecsSinceStartOfDay()) / 60000;


        // space between
        if(lastTrip && ui->cbShowBreaks->isChecked()) {
            ui->twTour->insertRow(targetRow);

            tourTableReference << lastTrip;

            QString time = lastTrip->endTime().toString("hh:mm") + "\r\n" + t->startTime().toString("hh:mm");
            QString route;
            QString busstops;
            QColor color;
            QColor cColor;

            if(lastTrip->route()->lastBusstop()->id() == t->route()->firstBusstop()->id()) {
                route = "break";
                busstops = lastTrip->route()->lastBusstop()->name();
                color = Qt::black;
                cColor = Qt::white;
            } else {
                route = "service trip";
                busstops = lastTrip->route()->lastBusstop()->name() + "\r\n" + t->route()->firstBusstop()->name();
                color = Qt::lightGray;
                cColor = Qt::black;
            }
            if(breakDuration < 0) {
                color = Qt::red;
                cColor = Qt::white;
            }

            ui->twTour->setItem(targetRow, 0, new QTableWidgetItem(time));
            ui->twTour->setItem(targetRow, 1, new QTableWidgetItem(QString::number(breakDuration) + " min."));
            ui->twTour->setItem(targetRow, 2, new QTableWidgetItem(route));
            ui->twTour->setItem(targetRow, 3, new QTableWidgetItem(busstops));

            if(ui->cbDifferentHeights->isChecked())
                ui->twTour->setRowHeight(targetRow, breakDuration * (ui->hsTourScale->value() / 5.0));
            else
                ui->twTour->setRowHeight(targetRow, 2.5 * (ui->hsTourScale->value()));

            for(int j = 0; j < 4; j++) {
                ui->twTour->item(targetRow, j)->setBackground(color);
                ui->twTour->item(targetRow, j)->setForeground(cColor);
                ui->twTour->item(targetRow, j)->setFont(italic);
            }
        }

        // next Trip
        lastTrip = t;

        tourTableReference << t;

        targetRow = ui->twTour->rowCount();
        ui->twTour->insertRow(targetRow);
        
        QColor color = projectData->lineOfRoute(t->route())->getColor();
        QColor contrastColor = global::getContrastColor(color);
        
        int duration = t->duration().msecsSinceStartOfDay() / 60000;
        lines << projectData->lineOfRoute(t->route())->name();
        
        ui->twTour->setItem(targetRow, 0, new QTableWidgetItem(t->startTime().toString("hh:mm") + "\r\n" + t->endTime().toString("hh:mm")));
        ui->twTour->setItem(targetRow, 1, new QTableWidgetItem(QString::number(duration) + " min."));
        ui->twTour->setItem(targetRow, 2, new QTableWidgetItem(t->route()->name() + "\r\n(" + t->timeProfile()->name() + ")"));
        ui->twTour->setItem(targetRow, 3, new QTableWidgetItem(t->route()->firstBusstop()->name() + "\r\n" + t->route()->lastBusstop()->name()));

        if(ui->cbDifferentHeights->isChecked())
            ui->twTour->setRowHeight(targetRow, duration * (ui->hsTourScale->value() / 5.0));
        else
            ui->twTour->setRowHeight(targetRow, 5 * (ui->hsTourScale->value()));

        for(int j = 0; j < 4; j++) {
            ui->twTour->item(targetRow, j)->setBackground(color);
            ui->twTour->item(targetRow, j)->setForeground(contrastColor);
            ui->twTour->item(targetRow, j)->setFont(bold);
        }

        if(lastCurrentTrip == t) {
            ui->twTour->setCurrentCell(targetRow, 3);
        }

        ui->twTour->verticalScrollBar()->setValue(scrollValue);
    }

    lines.removeDuplicates();

    ui->lLines->setText(lines.join(", "));
    ui->lStartTime->setText(m_currentTour->startTime().toString("hh:mm"));
    ui->lEndTime->setText(m_currentTour->endTime().toString("hh:mm"));
    ui->lTotalTime->setText(m_currentTour->duration().toString("hh:mm"));
    ui->lDrivingTime->setText(m_currentTour->drivingTime().toString("hh:mm"));
    ui->lBreakTime->setText(m_currentTour->breakTime().toString("hh:mm"));
}

/*void WdgTourEditor::refreshTour(Tour *t) {
    setCurrentTour
}*/

void WdgTourEditor::on_twTour_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous);

    if(!current) {
        m_currentTrip = nullptr;
        return;
    }

    if(current->row() != 2) {
        ui->twTour->setCurrentCell(current->row(), 2);
    }

    m_currentTrip = tourTableReference[current->row()];
    refreshTourNextTrips();
}


void WdgTourEditor::refreshTourNextTrips()
{
    ui->twNextTrips->clear();
    nextTripsListReference.clear();

    if(!m_currentTour || !m_currentTrip)
        return;

    QList<Trip *> resultList;
    
    for(int i = 0; i < projectData->lineCount(); i++) {
        Line *l = projectData->lineAt(i);
        QList<Trip *> trips = l->trips();

        for(int j = 0; j < trips.count(); j++) {
            Trip *t = trips[j];

            if(!m_currentTour->weekDays()->isIn(*t->weekDays()))
                continue;

            if(t->route()->firstBusstop()->id() != m_currentTrip->route()->lastBusstop()->id())
                continue;

            if(!t->hasRepeat()) {
                if(t->startTime() >= m_currentTrip->endTime())
                    resultList << t;
            } else {
                for(int k = 0; k < t->repeatTimes(); k++) {
                    if(t->repetitionAt(k)->startTime() >= m_currentTrip->endTime())
                        resultList << t->repetitionAt(k);
                }
            }
        }
    }

    resultList = ProjectData::sortTrips(resultList);

    QList<Trip *> resultListAdd;

    if(m_currentTrip->startTime() >= QTime(12, 0, 0, 0)) {
        for(int i = 0; i < projectData->lineCount(); i++) {
            Line *l = projectData->lineAt(i);
            QList<Trip *> trips = l->trips();
            for(int j = 0; j < trips.count(); j++) {
                Trip *t = trips[j];

                if(t->startTime() >= QTime(12, 0, 0, 0))
                    continue;

                if(!m_currentTour->weekDays()->shfitedToNextDay().isIn(*t->weekDays())) {
                    continue;
                }

                if(t->route()->firstBusstop()->id() != m_currentTrip->route()->lastBusstop()->id())
                    continue;

                if(!t->hasRepeat()) {
                        resultListAdd << t;
                } else {
                    for(int k = 0; k < t->repeatTimes(); k++) {
                        if(t->repetitionAt(k)->startTime() >= m_currentTrip->endTime())
                            resultListAdd << t->repetitionAt(k);
                    }
                }
            }
        }
    }

    resultListAdd = ProjectData::sortTrips(resultListAdd);
    resultList << resultListAdd;

    for(int i = 0; i < resultList.count(); i++) {
        Trip *t = resultList[i];
        int minutes = (t->startTime().msecsSinceStartOfDay() - m_currentTrip->endTime().msecsSinceStartOfDay()) / 60000;

        if(minutes > 300)
            break;

        if(minutes < 0)
            minutes += 1440;

        nextTripsListReference << t;

        QString breakDuration = QString::number(minutes) + " min.";
        QTreeWidgetItem *itm = new QTreeWidgetItem(ui->twNextTrips);
        itm->setText(1, t->startTime().toString("hh:mm"));
        itm->setText(2, breakDuration);
        itm->setText(3, t->route()->name());

        if(i == 0)
            ui->twNextTrips->setCurrentItem(itm);

        QList<Tour *> tourList = projectData->toursOfTrip(t);
        if(!tourList.empty()) {
            itm->setForeground(3, QColor(255, 0, 255));
            QStringList toolTip;
            for(int j = 0; j < tourList.count(); j++) {
                toolTip << tourList[j]->name();
            }
            itm->setToolTip(3, toolTip.join(", "));
        }
    }
}

void WdgTourEditor::setCurrentTour(Tour *t) {
    m_currentTour = t;
    refreshTour();
}

Trip * WdgTourEditor::currentNextTrip()
{
    QTreeWidgetItem *itm = ui->twNextTrips->currentItem();
    if(!itm)
        return nullptr;

    QString id = itm->text(0);
    for(int i = 0; i < projectData->lineCount(); i++) {
        Line *l = projectData->lineAt(i);
        Trip *t = l->trip(id);
        if(t)
            return t;
    }

    return nullptr;
}

void WdgTourEditor::on_twNextTrips_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous);

    if(!current) {
        m_currentNextTrip = nullptr;
        return;
    }

    m_currentNextTrip = nextTripsListReference[ui->twNextTrips->indexOfTopLevelItem(current)];
}





































