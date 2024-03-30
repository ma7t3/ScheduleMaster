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
    DlgFootnoteEditor *dlg = new DlgFootnoteEditor(this, projectData);
    dlg->setCreateNewMode(true);
    dlg->exec();
    if(dlg->result() != QDialog::Accepted)
        return;

    Footnote *f = new Footnote(projectData, global::getNewID(), dlg->identifier(), dlg->name(), dlg->description());
    undoStack->push(new CmdFootnoteNew(projectData, f));

    refreshFootnotes();
}

void WdgFootnotes::actionEdit() {
    if(!ui->tableWidget->currentItem() || ui->tableWidget->selectionModel()->selectedRows().count() != 1)
        return;

    int index = ui->tableWidget->currentItem()->row();
    Footnote *f = projectData->footnoteAt(index);

    DlgFootnoteEditor *dlg = new DlgFootnoteEditor(this, projectData);
    dlg->setCreateNewMode(false);

    dlg->setIdentifier(f->identifier());
    dlg->setName(f->name());
    dlg->setDescription(f->description());

    dlg->exec();
    if(dlg->result() != QDialog::Accepted)
        return;

    f->setIdentifier(dlg->identifier());
    f->setName(dlg->name());
    f->setDescription(dlg->description());

    refreshFootnotes();
}

void WdgFootnotes::actionDuplicate() {
    if(!ui->tableWidget->currentItem())
        return;

    int index = ui->tableWidget->currentItem()->row();
    Footnote *f = projectData->footnoteAt(index);

    DlgFootnoteEditor *dlg = new DlgFootnoteEditor(this, projectData);
    dlg->setCreateNewMode(false);

    dlg->setIdentifier(f->identifier());
    dlg->setName(f->name());
    dlg->setDescription(f->description());

    dlg->exec();
    if(dlg->result() != QDialog::Accepted)
        return;

    Footnote *newF = new Footnote(projectData, global::getNewID(), dlg->identifier(), dlg->name(), dlg->description());
    undoStack->push(new CmdFootnoteNew(projectData, newF));

    refreshFootnotes();
}

void WdgFootnotes::actionDelete() {
    QList<Footnote *> selectedFootnotes;
    QString stringList;
    for(int i = 0; i < ui->tableWidget->rowCount(); i++) {
        if(ui->tableWidget->item(i, 0)->isSelected()) {
            Footnote *f = projectData->footnoteAt(i);
            selectedFootnotes << f;
            stringList += ("<li>" + f->name() + "</li>");
        }
    }

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Delete Footnote(s)"), tr("<p><b>Do you really want to delete these footnotes?</b></p><ul>%1</ul>").arg(stringList), QMessageBox::Yes|QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    undoStack->push(new CmdFootnotesDelete(projectData, selectedFootnotes));

    refreshFootnotes();
}

void WdgFootnotes::refreshFootnotes() {
    QList<Footnote *> footnotes = ProjectData::sortItems(projectData->footnotes());

    ui->tableWidget->setRowCount(0);

    int targetRow = 0;
    for(int i = 0; i < footnotes.count(); i++) {
        Footnote *f = footnotes[i];

        ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);

        ui->tableWidget->setItem(targetRow, 0, new QTableWidgetItem(f->identifier()));
        ui->tableWidget->setItem(targetRow, 1, new QTableWidgetItem(f->name()));
        ui->tableWidget->setItem(targetRow, 2, new QTableWidgetItem(f->description()));

        targetRow++;
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

