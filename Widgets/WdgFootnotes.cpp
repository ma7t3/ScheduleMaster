#include "WdgFootnotes.h"
#include "ui_WdgFootnotes.h"

#include "App/global.h"
#include "projectData/footnote.h"

#include <QMessageBox>

#include "Dialogs/DlgFootnoteEditor.h"
#include <QObject>

#include "Commands/CmdFootnotes.h"

WdgFootnotes::WdgFootnotes(QWidget *parent, ProjectData *projectData, QUndoStack *undoStack) :
    QWidget(parent), ui(new Ui::WdgFootnotes),
    projectData(projectData), undoStack(undoStack) {

    ui->setupUi(this);

    ui->tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);

    connect(ui->pbNew,       &QPushButton::clicked,            this, &WdgFootnotes::actionNew);
    connect(ui->pbEdit,      &QPushButton::clicked,            this, &WdgFootnotes::actionEdit);
    connect(ui->tableWidget, &QTableWidget::itemDoubleClicked, this, &WdgFootnotes::actionEdit);
    connect(ui->pbDelete,    &QPushButton::clicked,            this, &WdgFootnotes::actionDelete);

    refreshFootnotes();
    refreshUI();
}

WdgFootnotes::~WdgFootnotes() {
    delete ui;
}

void WdgFootnotes::actionNew() {
    DlgFootnoteEditor dlg(this, projectData);
    dlg.setCreateNewMode(true);
    dlg.exec();
    if(dlg.result() != QDialog::Accepted)
        return;

    Footnote *f = projectData->newFootnote();
    f->setIdentifier(dlg.identifier());
    f->setName(dlg.name());
    f->setDescription(dlg.description());
    f->setAutoAssignWeekDaysEnabled(dlg.autoAssignWeekDaysEnabled());
    f->setAutoAssignWeekDays(dlg.weekDays());
    f->setAutoAssignCareWeekDays(dlg.careWeekDays());

    undoStack->push(new CmdFootnoteNew(projectData, f));

    emit refreshRequested();
}

void WdgFootnotes::actionEdit() {
    if(!ui->tableWidget->currentItem() || ui->tableWidget->selectionModel()->selectedRows().count() != 1)
        return;

    int index = ui->tableWidget->currentItem()->row();
    Footnote *f = _footnotesReference[index];

    DlgFootnoteEditor dlg(this, projectData);
    dlg.setCreateNewMode(false);

    dlg.setIdentifier(f->identifier());
    dlg.setName(f->name());
    dlg.setDescription(f->description());

    dlg.setAutoAssignWeekDaysEnabled(f->autoAssignWeekDaysEnabled());
    dlg.setWeekDays(f->autoAssignWeekDays());
    dlg.setCareWeekDays(f->autoAssignCareWeekDays());

    dlg.exec();
    if(dlg.result() != QDialog::Accepted)
        return;

    Footnote newF(*f);

    newF.setIdentifier(dlg.identifier());
    newF.setName(dlg.name());
    newF.setDescription(dlg.description());

    newF.setAutoAssignWeekDaysEnabled(dlg.autoAssignWeekDaysEnabled());
    newF.setAutoAssignWeekDays(dlg.weekDays());
    newF.setAutoAssignCareWeekDays(dlg.careWeekDays());

    undoStack->push(new CmdFootnoteEdit(f, newF));

    emit refreshRequested();
}

void WdgFootnotes::actionDuplicate() {
    if(!ui->tableWidget->currentItem())
        return;

    int index = ui->tableWidget->currentItem()->row();
    Footnote *f = _footnotesReference[index];

    DlgFootnoteEditor dlg(this, projectData);
    dlg.setCreateNewMode(false);

    dlg.setIdentifier(f->identifier());
    dlg.setName(f->name());
    dlg.setDescription(f->description());

    dlg.setAutoAssignWeekDaysEnabled(f->autoAssignWeekDaysEnabled());
    dlg.setWeekDays(f->autoAssignWeekDays());
    dlg.setCareWeekDays(f->autoAssignCareWeekDays());

    dlg.exec();
    if(dlg.result() != QDialog::Accepted)
        return;

    Footnote *newF = projectData->newFootnote();
    newF->setIdentifier(dlg.identifier());
    newF->setName(dlg.name());
    newF->setDescription(dlg.description());

    newF->setAutoAssignWeekDaysEnabled(dlg.autoAssignWeekDaysEnabled());
    newF->setAutoAssignWeekDays(dlg.weekDays());
    newF->setAutoAssignCareWeekDays(dlg.careWeekDays());

    undoStack->push(new CmdFootnoteNew(projectData, newF));

    emit refreshRequested();
}

void WdgFootnotes::actionDelete() {
    QString stringList;

    QList<Footnote *> selectedFootnotes;

    for(int i = 0; i < ui->tableWidget->rowCount(); i++) {
        if(ui->tableWidget->item(i, 0)->isSelected()) {
            Footnote *f = _footnotesReference[i];
            selectedFootnotes << f;
            stringList += ("<li>" + f->name() + "</li>");
        }
    }

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete Footnote(s)"), tr("<p><b>Do you really want to delete these footnotes?</b></p><ul>%1</ul>").arg(stringList), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    undoStack->push(new CmdFootnotesDelete(projectData, selectedFootnotes));

    emit refreshRequested();
}

void WdgFootnotes::refreshFootnotes() {
    QList<Footnote *> footnotes = ProjectData::sortItems(projectData->footnotes());

    ui->tableWidget->setRowCount(0);
    _footnotesReference.clear();

    int targetRow = 0;
    for(int i = 0; i < footnotes.count(); i++) {
        Footnote *f = footnotes[i];

        ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);

        ui->tableWidget->setItem(targetRow, 0, new QTableWidgetItem(f->identifier()));
        ui->tableWidget->setItem(targetRow, 1, new QTableWidgetItem(f->name()));
        ui->tableWidget->setItem(targetRow, 2, new QTableWidgetItem(f->description()));

        targetRow++;

        _footnotesReference << f;
    }

    ui->tableWidget->resizeColumnsToContents();
}

void WdgFootnotes::refreshUI() {
    int selectedRowsCount = ui->tableWidget->selectionModel()->selectedRows().count();

    if(selectedRowsCount == 0) {
        ui->pbEdit->setEnabled(false);
        ui->pbDelete->setEnabled(false);
    }

    if(selectedRowsCount == 1) {
        ui->pbEdit->setEnabled(true);
        ui->pbDelete->setEnabled(true);
    }

    if(selectedRowsCount > 1) {
        ui->pbEdit->setEnabled(false);
        ui->pbDelete->setEnabled(true);
    }
}

void WdgFootnotes::on_tableWidget_itemSelectionChanged() {
    refreshUI();
}

