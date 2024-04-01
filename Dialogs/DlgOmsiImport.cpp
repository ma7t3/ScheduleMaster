#include "DlgOmsiImport.h"
#include "ui_DlgOmsiImport.h"

#include "Plugins/PlgOmsiImport.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QCloseEvent>

DlgOmsiImport::DlgOmsiImport(QWidget *parent, ProjectData *projectData) :
    QDialog(parent),
    ui(new Ui::DlgOmsiImport),
    _importer(new PlgOmsiImport(this, projectData)),
    projectData(projectData) {
    ui->setupUi(this);

    connect(ui->lwTrips, &QListWidget::itemClicked, this, &DlgOmsiImport::refreshCount);
    connect(ui->lwLines, &QListWidget::itemClicked, this, &DlgOmsiImport::refreshCount);

    connect(_importer, &PlgOmsiImport::newFileStarted, this, &DlgOmsiImport::refreshProgress);
    connect(_importer, &QThread::finished, this, &DlgOmsiImport::finish);

    setWindowFlag(Qt::WindowContextHelpButtonHint);
}

DlgOmsiImport::~DlgOmsiImport() {
    delete ui;
}

void DlgOmsiImport::closeEvent(QCloseEvent *e) {
    if(_importer->isRunning())
        e->ignore();
    else
        e->accept();
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

void DlgOmsiImport::on_pbStart_clicked() {
    ui->gbMapDir ->setEnabled(false);
    ui->gbOptions->setEnabled(false);
    ui->gbTrips  ->setEnabled(false);
    ui->gbLines  ->setEnabled(false);
    ui->pbStart  ->setEnabled(false);

    QStringList trips, lines;

    for(int i = 0; i < ui->lwTrips->count(); i++) {
        if(ui->lwTrips->item(i)->checkState() == Qt::Checked)
            trips << ui->lwTrips->item(i)->text();
    }

    for(int i = 0; i < ui->lwLines->count(); i++) {
        if(ui->lwLines->item(i)->checkState() == Qt::Checked)
            lines << ui->lwLines->item(i)->text();
    }

    _importer->setMapDirectory(ui->leDir->text());
    _importer->setTripMask(ui->leTripNameMask->text());
    _importer->setSelectedTrips(trips);
    _importer->setSelectedLines(lines);

    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(trips.count() + lines.count());
    _importer->start();
}

void DlgOmsiImport::refreshProgress(int index, QString filePath) {
    ui->lCurrentProgress->setText(filePath);
    ui->progressBar->setValue(index);
}

void DlgOmsiImport::finish() {
    ui->gbMapDir ->setEnabled(true);
    ui->gbOptions->setEnabled(true);
    ui->gbTrips  ->setEnabled(true);
    ui->gbLines  ->setEnabled(true);
    ui->pbStart  ->setEnabled(true);

    ui->progressBar->setValue(ui->progressBar->maximum());
    QMessageBox::information(this, tr("Imported successfully"), tr("Your OMSI map was imported sucessfully!"));
    accept();
}

void DlgOmsiImport::on_cbMergeDuplicateTrips_stateChanged(int arg1) {
    _importer->setMergeTrips(arg1 == Qt::Checked);
}

