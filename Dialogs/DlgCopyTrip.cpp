#include "DlgCopyTrip.h"
#include "ui_DlgCopyTrip.h"

DlgCopyTrip::DlgCopyTrip(QWidget *parent, const QTime &startTime) :
    QDialog(parent),
    ui(new Ui::DlgCopyTrip),
    _startTime(startTime)
{
    ui->setupUi(this);

    ui->teEndTime->setTime(_startTime);
    ui->lStartTime->setText(_startTime.toString("hh:mm"));

    ui->teInterval->setFocus();
}

DlgCopyTrip::~DlgCopyTrip()
{
    delete ui;
}

void DlgCopyTrip::setStartTime(const QTime &startTime) {
    _startTime = startTime;
    ui->teEndTime->setTime(_startTime);
    ui->lStartTime->setText(_startTime.toString("hh:mm"));
}

int DlgCopyTrip::copyCount() const {
    return ui->sbCount->value();
}

QTime DlgCopyTrip::interval() const {
    return ui->teInterval->time();
}

void DlgCopyTrip::refresh() {
    if(ui->rbCount->isChecked()) {
        ui->teEndTime->setTime(QTime::fromMSecsSinceStartOfDay(ui->sbCount->value() * ui->teInterval->time().msecsSinceStartOfDay() + _startTime.msecsSinceStartOfDay()));
    } else {
        int i = -1;
        QTime targetTime = _startTime;

        while(ui->teEndTime->time() >= targetTime) {
            i++;
            targetTime = targetTime.addMSecs(ui->teInterval->time().msecsSinceStartOfDay());
        }

        ui->sbCount->setValue(i);
    }
}

void DlgCopyTrip::on_rbEndTime_clicked() {
    ui->teEndTime->setEnabled(true);
    ui->sbCount->setEnabled(false);
}


void DlgCopyTrip::on_rbCount_clicked() {
    ui->teEndTime->setEnabled(false);
    ui->sbCount->setEnabled(true);
}


void DlgCopyTrip::on_teInterval_timeChanged(const QTime &time) {
    refresh();
}


void DlgCopyTrip::on_teEndTime_timeChanged(const QTime &time) {
    refresh();
}


void DlgCopyTrip::on_sbCount_valueChanged(int arg1) {
    refresh();
}

