#include "DlgFootnoteEditor.h"
#include "ui_DlgFootnoteEditor.h"

DlgFootnoteEditor::DlgFootnoteEditor(QWidget *parent, ProjectData *projectData) :
    QDialog(parent), ui(new Ui::DlgFootnoteEditor),
    projectData(projectData) {
    ui->setupUi(this);

    ui->leIdentifier->setFocus();

    ui->daySelector->setTristate(true);
}

DlgFootnoteEditor::~DlgFootnoteEditor() {
    delete ui;
}

void DlgFootnoteEditor::setCreateNewMode(bool create) {
    setWindowTitle(create ? tr("Create Footnote") : tr("Edit Footnote"));
}

QString DlgFootnoteEditor::identifier() const {
    return ui->leIdentifier->text();
}

void DlgFootnoteEditor::setIdentifier(const QString &newIdentifier) {
    ui->leIdentifier->setText(newIdentifier);
}

QString DlgFootnoteEditor::name() const {
    return ui->leName->text();
}

void DlgFootnoteEditor::setName(const QString &newName) {
    ui->leName->setText(newName);
}

QString DlgFootnoteEditor::description() const {
    return ui->leDescription->text();
}

void DlgFootnoteEditor::setDescription(const QString &newDescription) {
    ui->leDescription->setText(newDescription);
}

WeekDays DlgFootnoteEditor::weekDays() const {
    return ui->daySelector->weekDays();
}
void DlgFootnoteEditor::setWeekDays(const WeekDays &w) {
    ui->daySelector->setWeekDays(w);
}

WeekDays DlgFootnoteEditor::careWeekDays() const {
    return ui->daySelector->careWeekDays();
}

void DlgFootnoteEditor::setCareWeekDays(const WeekDays &w) {
    ui->daySelector->setCareWeekDays(w);
}

void DlgFootnoteEditor::setAutoAssignWeekDaysEnabled(const bool &b) {
    ui->gbAutoAssign->setChecked(b);
}

bool DlgFootnoteEditor::autoAssignWeekDaysEnabled() const {
    return ui->gbAutoAssign->isChecked();
}
