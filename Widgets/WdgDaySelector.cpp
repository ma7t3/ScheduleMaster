#include "WdgDaySelector.h"
#include "ui_WdgDaySelector.h"

WdgDaySelector::WdgDaySelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgDaySelector) {
    ui->setupUi(this);

    QObject::connect(ui->cbMonday, SIGNAL(stateChanged(int)), this, SLOT(emitChangedSignal()));
    QObject::connect(ui->cbTuesday, SIGNAL(stateChanged(int)), this, SLOT(emitChangedSignal()));
    QObject::connect(ui->cbWednesday, SIGNAL(stateChanged(int)), this, SLOT(emitChangedSignal()));
    QObject::connect(ui->cbThursday, SIGNAL(stateChanged(int)), this, SLOT(emitChangedSignal()));
    QObject::connect(ui->cbFriday, SIGNAL(stateChanged(int)), this, SLOT(emitChangedSignal()));
    QObject::connect(ui->cbSaturday, SIGNAL(stateChanged(int)), this, SLOT(emitChangedSignal()));
    QObject::connect(ui->cbSunday, SIGNAL(stateChanged(int)), this, SLOT(emitChangedSignal()));
    QObject::connect(ui->cbHoliday, SIGNAL(stateChanged(int)), this, SLOT(emitChangedSignal()));
    QObject::connect(ui->cbSchool, SIGNAL(stateChanged(int)), this, SLOT(emitChangedSignal()));
    QObject::connect(ui->cbVacation, SIGNAL(stateChanged(int)), this, SLOT(emitChangedSignal()));
}

WdgDaySelector::~WdgDaySelector() {
    delete ui;
}

void WdgDaySelector::setSmallMode(bool b) {
    if(b) {
        ui->cbMonday->setText(tr("Mon"));
        ui->cbTuesday->setText(tr("Tue"));
        ui->cbWednesday->setText(tr("Wed"));
        ui->cbThursday->setText(tr("Thu"));
        ui->cbFriday->setText(tr("Fri"));
        ui->cbSaturday->setText(tr("Sat"));
        ui->cbSunday->setText(tr("Sun"));
        ui->cbHoliday->setText(tr("Hol"));
    } else {
        ui->cbMonday->setText(tr("Monday"));
        ui->cbTuesday->setText(tr("Tuesday"));
        ui->cbWednesday->setText(tr("Wednesday"));
        ui->cbThursday->setText(tr("Thursday"));
        ui->cbFriday->setText(tr("Friday"));
        ui->cbSaturday->setText(tr("Saturday"));
        ui->cbSunday->setText(tr("Sunday"));
        ui->cbHoliday->setText(tr("Holiday"));
    }
}

void WdgDaySelector::setWeekDays(WeekDays w) {
    ui->cbMonday->setChecked(w.monday());
    ui->cbTuesday->setChecked(w.tuesday());
    ui->cbWednesday->setChecked(w.wednesday());
    ui->cbThursday->setChecked(w.thursday());
    ui->cbFriday->setChecked(w.friday());
    ui->cbSaturday->setChecked(w.saturday());
    ui->cbSunday->setChecked(w.sunday());
    ui->cbHoliday->setChecked(w.holiday());
    ui->cbSchool->setChecked(w.school());
    ui->cbVacation->setChecked(w.vacation());
}

WeekDays WdgDaySelector::weekDays() {
    WeekDays w;
    w.setMonday(ui->cbMonday->isChecked());
    w.setTuesday(ui->cbTuesday->isChecked());
    w.setWednesday(ui->cbWednesday->isChecked());
    w.setTuesday(ui->cbThursday->isChecked());
    w.setFriday(ui->cbFriday->isChecked());
    w.setSaturday(ui->cbSaturday->isChecked());
    w.setSunday(ui->cbSunday->isChecked());
    w.setHoliday(ui->cbHoliday->isChecked());
    w.setSchool(ui->cbSchool->isChecked());
    w.setVacation(ui->cbVacation->isChecked());

    return w;
}

void WdgDaySelector::emitChangedSignal() {
    emit weekDaysChanged();
}
