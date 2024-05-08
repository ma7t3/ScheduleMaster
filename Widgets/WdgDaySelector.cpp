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

void WdgDaySelector::setSmallMode(const bool &b) {
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

void WdgDaySelector::setTristate(const bool &b) {
    ui->cbMonday    ->setTristate(b);
    ui->cbTuesday   ->setTristate(b);
    ui->cbWednesday ->setTristate(b);
    ui->cbThursday  ->setTristate(b);
    ui->cbFriday    ->setTristate(b);
    ui->cbSaturday  ->setTristate(b);
    ui->cbSunday    ->setTristate(b);
    ui->cbHoliday   ->setTristate(b);
    ui->cbSchool    ->setTristate(b);
    ui->cbVacation  ->setTristate(b);
}

void WdgDaySelector::setWeekDays(const WeekDays  &w) {
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

void WdgDaySelector::setCareWeekDays(const WeekDays &w) {
    if(!w.day(WeekDay::monday))    { ui->cbMonday    ->setCheckState(Qt::PartiallyChecked);}
    if(!w.day(WeekDay::tuesday))   { ui->cbTuesday   ->setCheckState(Qt::PartiallyChecked);}
    if(!w.day(WeekDay::wednesday)) { ui->cbWednesday ->setCheckState(Qt::PartiallyChecked);}
    if(!w.day(WeekDay::thursday))  { ui->cbThursday  ->setCheckState(Qt::PartiallyChecked);}
    if(!w.day(WeekDay::friday))    { ui->cbFriday    ->setCheckState(Qt::PartiallyChecked);}
    if(!w.day(WeekDay::saturday))  { ui->cbSaturday  ->setCheckState(Qt::PartiallyChecked);}
    if(!w.day(WeekDay::sunday))    { ui->cbSunday    ->setCheckState(Qt::PartiallyChecked);}
    if(!w.day(WeekDay::holiday))   { ui->cbHoliday   ->setCheckState(Qt::PartiallyChecked);}
    if(!w.day(WeekDay::school))    { ui->cbSchool    ->setCheckState(Qt::PartiallyChecked);}
    if(!w.day(WeekDay::vacation))  { ui->cbVacation  ->setCheckState(Qt::PartiallyChecked);}
}

WeekDays WdgDaySelector::weekDays() const {
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

WeekDays WdgDaySelector::careWeekDays() const {
    WeekDays w(nullptr);
    w.setDay(WeekDay::monday,    ui->cbMonday->checkState()    != Qt::PartiallyChecked);
    w.setDay(WeekDay::tuesday,   ui->cbTuesday->checkState()   != Qt::PartiallyChecked);
    w.setDay(WeekDay::wednesday, ui->cbWednesday->checkState() != Qt::PartiallyChecked);
    w.setDay(WeekDay::thursday,  ui->cbThursday->checkState()  != Qt::PartiallyChecked);
    w.setDay(WeekDay::friday,    ui->cbFriday->checkState()    != Qt::PartiallyChecked);
    w.setDay(WeekDay::saturday,  ui->cbSaturday->checkState()  != Qt::PartiallyChecked);
    w.setDay(WeekDay::sunday,    ui->cbSunday->checkState()    != Qt::PartiallyChecked);
    w.setDay(WeekDay::holiday,   ui->cbHoliday->checkState()   != Qt::PartiallyChecked);
    w.setDay(WeekDay::school,    ui->cbSchool->checkState()    != Qt::PartiallyChecked);
    w.setDay(WeekDay::vacation,  ui->cbVacation->checkState()  != Qt::PartiallyChecked);

    return w;
}

void WdgDaySelector::emitChangedSignal() {
    emit weekDaysChanged();
}
