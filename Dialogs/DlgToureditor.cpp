#include "Dialogs\DlgToureditor.h"
#include "ui_DlgToureditor.h"

TourEditor::TourEditor(QWidget *parent, bool createMode, QString name, WeekDays weekDays) :
    QDialog(parent),
    ui(new Ui::TourEditor) {
    ui->setupUi(this);
    setName(name);
    setWeekDays(weekDays);

    if(createMode)
        QDialog::setWindowTitle(tr("create tour"));
}

TourEditor::~TourEditor() {
    delete ui;
}

void TourEditor::setName(QString name) {
    ui->leName->setText(name);
}

void TourEditor::setWeekDays(WeekDays weekDays) {
    ui->daySelector->setWeekDays(weekDays);
}

QString TourEditor::name() {
    return ui->leName->text();
}

WeekDays TourEditor::weekDays() {
    return ui->daySelector->weekDays();
}


/*void TourEditor::on_pbMonFri_clicked() {
    ui->cbMon->setChecked(true);
    ui->cbTue->setChecked(true);
    ui->cbWed->setChecked(true);
    ui->cbThu->setChecked(true);
    ui->cbFri->setChecked(true);
    ui->cbSat->setChecked(false);
    ui->cbSun->setChecked(false);
    ui->cbHol->setChecked(false);
}

void TourEditor::on_pbSat_clicked() {
    ui->cbMon->setChecked(false);
    ui->cbTue->setChecked(false);
    ui->cbWed->setChecked(false);
    ui->cbThu->setChecked(false);
    ui->cbFri->setChecked(false);
    ui->cbSat->setChecked(true);
    ui->cbSun->setChecked(false);
    ui->cbHol->setChecked(false);
}


void TourEditor::on_pbSun_clicked() {
    ui->cbMon->setChecked(false);
    ui->cbTue->setChecked(false);
    ui->cbWed->setChecked(false);
    ui->cbThu->setChecked(false);
    ui->cbFri->setChecked(false);
    ui->cbSat->setChecked(false);
    ui->cbSun->setChecked(true);
    ui->cbHol->setChecked(true);
}*/















