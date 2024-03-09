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

    ui->leName->setFocus();
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
