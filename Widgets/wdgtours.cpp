#include "wdgtours.h"
#include "ui_wdgtours.h"

#include <QMessageBox>
#include <QInputDialog>

#include "App/global.h"
#include "Dialogs/DlgToureditor.h"
#include "Commands/cmdTours.h"
#include "Dialogs/DlgDataexporter.h"

WdgTours::WdgTours(QWidget *parent, ProjectData *projectData, QUndoStack *undoStack) :
    QWidget(parent),
    ui(new Ui::WdgTours),
    projectData(projectData),
    undoStack(undoStack)
{
    ui->setupUi(this);

    QObject::connect(ui->pbTourNew, SIGNAL(clicked()), this, SLOT(actionTourNew()));
    QObject::connect(ui->pbTourEdit, SIGNAL(clicked()), this, SLOT(actionTourEdit()));
    QObject::connect(ui->twTours, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(actionTourEdit()));
    QObject::connect(ui->pbTourDuplicate, SIGNAL(clicked()), this, SLOT(actionTourDuplicate()));
    QObject::connect(ui->pbTourDelete, SIGNAL(clicked()), this, SLOT(actionTourDelete()));
    QObject::connect(ui->pbExport, SIGNAL(clicked()), this, SLOT(actionExport()));
    QObject::connect(ui->leSearch, SIGNAL(textChanged(QString)), this, SLOT(refreshTourList()));

    ui->twTours->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->twTours->verticalHeader()->setVisible(false);

    ui->twTours->setColumnWidth(0, 150);
    ui->twTours->setColumnWidth(1, 75);
    ui->twTours->setColumnWidth(2, 25);
    ui->twTours->setColumnWidth(3, 80);
    ui->twTours->setColumnWidth(4, 150);
}

WdgTours::~WdgTours()
{
    delete ui;
}

void WdgTours::actionTourNew() {
    TourEditor dlg(this, true);
    dlg.exec();

    if(dlg.result() != QDialog::Accepted)
        return;

    Tour *o = new Tour(global::getNewID(), dlg.name(), dlg.getWeekDays());
    undoStack->push(new cmdTourNew(projectData, o));
    refreshTourList();
}

void WdgTours::actionTourEdit() {
    Tour *o = currentTour();
    if(!o)
        return;
    
    TourEditor dlg(this, false, o->name(), *o->weekDays());
    dlg.exec();

    if(dlg.result() != QDialog::Accepted)
        return;

    Tour newO = *o;

    newO.setName(dlg.name());
    newO.setWeekDays(dlg.getWeekDays());
    undoStack->push(new cmdTourEdit(o, newO));
    refreshTourList();
}

void WdgTours::actionTourDuplicate() {
    Tour *o = currentTour();
    if(!o)
        return;
    
    TourEditor dlg(this, false, o->name(), *o->weekDays());
    dlg.exec();

    if(dlg.result() != QDialog::Accepted)
        return;

    Tour *nO = new Tour(global::getNewID(), dlg.name(), dlg.getWeekDays());
    undoStack->push(new cmdTourNew(projectData, nO));
    refreshTourList();
}

void WdgTours::actionTourDelete() {
    Tour *o = currentTour();
    if(!o)
        return;

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete tour"), tr("Do you really want to delete this tour?"), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    undoStack->push(new cmdTourDelete(projectData, o));
    refreshTourList();
}

void WdgTours::actionExport() {
    bool ok = false;
    QString aiGroupName = QInputDialog::getText(this, "AI-Group", "Enter target AI-Group name", QLineEdit::Normal, "GVB - Gelenk", &ok);
    if(!ok)
        return;

    QString result = "";

    for(int i = 0; i < tableReference.count(); i++) {
        Tour *t = tableReference[i];

        result += "[newtour]\r\n" + t->name() + "\r\n" + aiGroupName + "\r\n799\r\n\r\n";

        QList<Trip *> trips = t->trips();
        for(int j = 0; j < trips.count(); j++) {
            Trip *t = trips[j];
            int timeProfileIndex = 0;
            int time = t->startTime().msecsSinceStartOfDay() / 1000 / 60;
            Route *r = t->route();
            for(int k = 0; k < r->timeProfileCount(); k++) {
                if(r->timeProfileAt(k) == t->timeProfile()) {
                    timeProfileIndex = k;
                    break;
                }
            }
            result += "  Dep.: " + t->startTime().toString("hh:mm:ss") + "\r\n[addtrip]\r\n" + t->route()->name() + "\r\n" + QString::number(timeProfileIndex) + "\r\n" + QString::number(time) + "\r\n\r\n";
        }
    }

    DataExporter dlg;
    dlg.setFileName("all_tours");
    dlg.setFolderName("tours");
    dlg.setText(result, "", "");
    dlg.exec();
}

Tour * WdgTours::currentTour() {
    return m_currentTour;
}

void WdgTours::refreshTourList()
{
    ui->twTours->clearContents();
    ui->twTours->setRowCount(0);
    tableReference.clear();

    QFont bold;
    bold.setBold(true);

    QList<Tour *> tours = projectData->tours();
    tours = ProjectData::sortTours(tours);

    QRegularExpression regex(ui->leSearch->text());
    regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    
    for(int i = 0; i < tours.count(); i++) {
        Tour *o = tours[i];

        if(!regex.match(o->name()).hasMatch())
            continue;

        int row = ui->twTours->rowCount();

        ui->twTours->insertRow(row);

        tableReference << o;
        ui->twTours->setItem(row, 0, new QTableWidgetItem(o->name()));
        ui->twTours->setItem(row, 1, new QTableWidgetItem(o->startTime().toString("hh:mm") + " - " + o->endTime().toString("hh:mm")));
        ui->twTours->setItem(row, 2, new QTableWidgetItem(o->duration().toString("hh:mm")));
        ui->twTours->setItem(row, 3, new QTableWidgetItem(o->weekDays()->toString()));
        ui->twTours->item(row, 0)->setFont(bold);

        int startTime = o->startTime().msecsSinceStartOfDay(), endTime = o->endTime().msecsSinceStartOfDay(), diff = 0, time = 0;

        if(endTime < startTime)
            endTime += 86400000;

        diff = endTime - startTime;
        time = startTime + diff;

        if(time > 86400000)
            time -= 86400000;

        int timeColorCode = 360 - (time / 240000);
        QColor timeColor;
        timeColor.setHsv(timeColorCode, 128, 255);
        ui->twTours->item(row, 1)->setBackground(timeColor);
        ui->twTours->setRowHeight(row, 20);


        int durationColorCode = 255 - o->duration().msecsSinceStartOfDay() / 168750;
        if(durationColorCode < 0)
            durationColorCode = 0;

        QColor durationColor;
        durationColor.setHsv(0, 0, durationColorCode);
        ui->twTours->item(row, 2)->setBackground(durationColor);
        if(durationColorCode < 128)
            ui->twTours->item(row, 2)->setForeground(Qt::white);

        QStringList lines;

        for(int j = 0; j < o->tripCount(); j++) {
            QString lineName = projectData->lineOfTrip(o->tripAt(j))->name();
            if(!lines.contains(lineName))
                lines << lineName;
        }

        ui->twTours->setItem(i, 4, new QTableWidgetItem(lines.join(", ")));
    }
}

void WdgTours::on_twTours_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous) {
    Q_UNUSED(previous);

    if(!current) {
        m_currentTour = nullptr;
        return;
    }

    if(current->column() != 0) {
        ui->twTours->setCurrentCell(current->row(), 0);
        return;
    }

    m_currentTour = tableReference[current->row()];
    emit currentTourChanged(m_currentTour);
}
