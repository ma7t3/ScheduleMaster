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
    ui->cbMonday    ->setChecked(w.day(WeekDay::monday));
    ui->cbTuesday   ->setChecked(w.day(WeekDay::tuesday));
    ui->cbWednesday ->setChecked(w.day(WeekDay::wednesday));
    ui->cbThursday  ->setChecked(w.day(WeekDay::thursday));
    ui->cbFriday    ->setChecked(w.day(WeekDay::friday));
    ui->cbSaturday  ->setChecked(w.day(WeekDay::saturday));
    ui->cbSunday    ->setChecked(w.day(WeekDay::sunday));
    ui->cbHoliday   ->setChecked(w.day(WeekDay::holiday));
    ui->cbSchool    ->setChecked(w.day(WeekDay::school));
    ui->cbVacation  ->setChecked(w.day(WeekDay::vacation));
}

WeekDays WdgDaySelector::weekDays() {
    WeekDays w(nullptr);
    w.setDay(WeekDay::monday,    ui->cbMonday->isChecked());
    w.setDay(WeekDay::tuesday,   ui->cbTuesday->isChecked());
    w.setDay(WeekDay::wednesday, ui->cbWednesday->isChecked());
    w.setDay(WeekDay::thursday,  ui->cbThursday->isChecked());
    w.setDay(WeekDay::friday,    ui->cbFriday->isChecked());
    w.setDay(WeekDay::saturday,  ui->cbSaturday->isChecked());
    w.setDay(WeekDay::sunday,    ui->cbSunday->isChecked());
    w.setDay(WeekDay::holiday,   ui->cbHoliday->isChecked());
    w.setDay(WeekDay::school,    ui->cbSchool->isChecked());
    w.setDay(WeekDay::vacation,  ui->cbVacation->isChecked());

    return w;
}

void WdgDaySelector::emitChangedSignal() {
    emit weekDaysChanged();
}
