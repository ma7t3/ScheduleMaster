#include "Dialogs\DlgToureditor.h"
#include "ui_DlgToureditor.h"

TourEditor::TourEditor(QWidget *parent, bool createMode, QString name, WeekDays weekDays, QList<DayType *> dayTypes) :
    QDialog(parent),
    ui(new Ui::TourEditor),
    _dayTypes(dayTypes) {
    ui->setupUi(this);
    setName(name);
    setWeekDays(weekDays);

    if(createMode)
        QDialog::setWindowTitle(tr("create tour"));

    ui->leName->setFocus();

    for(int i = 0; i < dayTypes.count(); i++)
        ui->cbDayTypes->addItem(dayTypes[i]->name());

    refreshDayTypeSelector();

    connect(ui->daySelector, &WdgDaySelector::weekDaysChanged, this, &TourEditor::refreshDayTypeSelector);
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

void TourEditor::on_cbDayTypes_activated(int index) {
    setWeekDays(*_dayTypes[index]);
}

void TourEditor::refreshDayTypeSelector() {
    WeekDays w = ui->daySelector->weekDays();
    ui->cbDayTypes->setCurrentIndex(-1);

    for(int i = 0; i < _dayTypes.count(); i++)
        if(w == *_dayTypes[i])
            ui->cbDayTypes->setCurrentIndex(i);
}
