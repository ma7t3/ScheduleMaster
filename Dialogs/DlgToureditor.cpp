#include "Dialogs\DlgToureditor.h"
#include "ui_DlgToureditor.h"

TourEditor::TourEditor(QWidget *parent, bool createMode, QString name, WeekDays weekDays) :
    QDialog(parent),
    ui(new Ui::TourEditor)
{
    ui->setupUi(this);
    setName(name);
    setWeekDays(weekDays);

    if(createMode)
        QDialog::setWindowTitle(tr("create tour"));
}

TourEditor::~TourEditor()
{
    delete ui;
}

void TourEditor::setName(QString name)
{
    ui->leName->setText(name);
}

void TourEditor::setWeekDays(WeekDays weekDays)
{
    ui->cbMon->setChecked(weekDays.monday());
    ui->cbTue->setChecked(weekDays.tuesday());
    ui->cbWed->setChecked(weekDays.wednesday());
    ui->cbThu->setChecked(weekDays.thursday());
    ui->cbFri->setChecked(weekDays.friday());
    ui->cbSat->setChecked(weekDays.saturday());
    ui->cbSun->setChecked(weekDays.sunday());
    ui->cbHol->setChecked(weekDays.holiday());
    ui->cbSchool->setChecked(weekDays.school());
    ui->cbNoSchool->setChecked(weekDays.vacation());
}

QString TourEditor::name()
{
    return ui->leName->text();
}

WeekDays TourEditor::getWeekDays()
{
    WeekDays w;

    w.setMonday(ui->cbMon->isChecked());
    w.setTuesday(ui->cbTue->isChecked());
    w.setWednesday(ui->cbWed->isChecked());
    w.setThursday(ui->cbThu->isChecked());
    w.setFriday(ui->cbFri->isChecked());
    w.setSaturday(ui->cbSat->isChecked());
    w.setSunday(ui->cbSun->isChecked());
    w.setHoliday(ui->cbHol->isChecked());
    w.setSchool(ui->cbSchool->isChecked());
    w.setVacation(ui->cbNoSchool->isChecked());

    return w;
}


void TourEditor::on_pbMonFri_clicked()
{
    ui->cbMon->setChecked(true);
    ui->cbTue->setChecked(true);
    ui->cbWed->setChecked(true);
    ui->cbThu->setChecked(true);
    ui->cbFri->setChecked(true);
    ui->cbSat->setChecked(false);
    ui->cbSun->setChecked(false);
    ui->cbHol->setChecked(false);
}

void TourEditor::on_pbSat_clicked()
{
    ui->cbMon->setChecked(false);
    ui->cbTue->setChecked(false);
    ui->cbWed->setChecked(false);
    ui->cbThu->setChecked(false);
    ui->cbFri->setChecked(false);
    ui->cbSat->setChecked(true);
    ui->cbSun->setChecked(false);
    ui->cbHol->setChecked(false);
}


void TourEditor::on_pbSun_clicked()
{
    ui->cbMon->setChecked(false);
    ui->cbTue->setChecked(false);
    ui->cbWed->setChecked(false);
    ui->cbThu->setChecked(false);
    ui->cbFri->setChecked(false);
    ui->cbSat->setChecked(false);
    ui->cbSun->setChecked(true);
    ui->cbHol->setChecked(true);
}















