#include "DlgOmsiImport.h"
#include "ui_DlgOmsiImport.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

DlgOmsiImport::DlgOmsiImport(QWidget *parent) : QDialog(parent), ui(new Ui::DlgOmsiImport) {
    ui->setupUi(this);

    connect(ui->lwTrips, &QListWidget::itemClicked, this, &DlgOmsiImport::refreshCount);
    connect(ui->lwLines, &QListWidget::itemClicked, this, &DlgOmsiImport::refreshCount);
}

DlgOmsiImport::~DlgOmsiImport() {
    delete ui;
}

void DlgOmsiImport::on_pbDirBrowse_clicked() {
    QString oldPath = ui->leDir->text();
    if(oldPath.isEmpty())
        oldPath = "C:/Program Files (x86)/Steam/steamapps/common/OMSI 2/maps";
    QString path = QFileDialog::getExistingDirectory(this, "", oldPath, QFileDialog::ShowDirsOnly);
    if(path.isEmpty())
        return;

    ui->leDir->setText(path);
    refresh();
}

void DlgOmsiImport::refresh() {
    ui->lwTrips->clear();
    ui->lwLines->clear();
    QDir dir(ui->leDir->text() + "/TTData");
    if(!dir.exists()) {
        QMessageBox::warning(this, tr("TTData not found"), tr("<p><b>No valid TTData directory found!</b></p><p>Cannot load trips and lines</p>"));
        return;
    }

    QStringList trips, lines;

    trips = dir.entryList({"*.ttp"}, QDir::Files);
    lines = dir.entryList({"*.ttl"}, QDir::Files);

    ui->lwTrips->addItems(trips);
    ui->lwLines->addItems(lines);

    for(int i = 0; i < ui->lwTrips->count(); i++) {
        ui->lwTrips->item(i)->setText(ui->lwTrips->item(i)->text().remove(".ttp"));
        ui->lwTrips->item(i)->setCheckState(Qt::Unchecked);
    }

    for(int i = 0; i < ui->lwLines->count(); i++) {
        ui->lwLines->item(i)->setText(ui->lwLines->item(i)->text().remove(".ttl"));
        ui->lwLines->item(i)->setCheckState(Qt::Unchecked);
    }

    refreshCount();
}

void DlgOmsiImport::refreshCount() {
    int selectedTripsCount = 0, selectedLinesCount = 0;

    for(int i = 0; i < ui->lwTrips->count(); i++)
        if(ui->lwTrips->item(i)->checkState() == Qt::Checked)
            selectedTripsCount++;

    for(int i = 0; i < ui->lwLines->count(); i++)
        if(ui->lwLines->item(i)->checkState() == Qt::Checked)
            selectedLinesCount++;

    ui->lTripCount->setText(tr("Count: %1 (%2 selected)").arg(ui->lwTrips->count()).arg(selectedTripsCount));
    ui->lLineCount->setText(tr("Count: %1 (%2 selected)").arg(ui->lwLines->count()).arg(selectedLinesCount));
}

void DlgOmsiImport::on_phTripsSelectAll_clicked() {
    for(int i = 0; i < ui->lwTrips->count(); i++)
        ui->lwTrips->item(i)->setCheckState(Qt::Checked);
    refreshCount();
}

void DlgOmsiImport::on_pbTripsDeselectAll_clicked() {
    for(int i = 0; i < ui->lwTrips->count(); i++)
        ui->lwTrips->item(i)->setCheckState(Qt::Unchecked);
    refreshCount();
}

void DlgOmsiImport::on_pbTripsToggleSelected_clicked() {
    for(int i = 0; i < ui->lwTrips->count(); i++) {
        QListWidgetItem *itm = ui->lwTrips->item(i);
        if(!itm->isSelected())
            continue;

        if(itm->checkState() == Qt::Checked)
            itm->setCheckState(Qt::Unchecked);
        else
            itm->setCheckState(Qt::Checked);
    }
    refreshCount();
}

void DlgOmsiImport::on_pbLinesSelectAll_clicked() {
    for(int i = 0; i < ui->lwLines->count(); i++)
        ui->lwLines->item(i)->setCheckState(Qt::Checked);
    refreshCount();
}

void DlgOmsiImport::on_pbLinesDeselectAll_clicked() {
    for(int i = 0; i < ui->lwLines->count(); i++)
        ui->lwLines->item(i)->setCheckState(Qt::Unchecked);
    refreshCount();
}

void DlgOmsiImport::on_pbLinesToggleSelected_clicked() {
    for(int i = 0; i < ui->lwLines->count(); i++) {
        QListWidgetItem *itm = ui->lwLines->item(i);
        if(!itm->isSelected())
            continue;

        if(itm->checkState() == Qt::Checked)
            itm->setCheckState(Qt::Unchecked);
        else
            itm->setCheckState(Qt::Checked);
    }
    refreshCount();
}
