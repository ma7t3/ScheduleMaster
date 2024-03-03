#include "DlgManageFootnotes.h"
#include "ui_DlgManageFootnotes.h"

#include "App/global.h"
#include "ProjectData/footnote.h"

#include "Dialogs/DlgFootnoteEditor.h"

DlgManageFootnotes::DlgManageFootnotes(QWidget *parent, ProjectData *projectData) :
    QDialog(parent), ui(new Ui::DlgManageFootnotes),
    projectData(projectData) {
    ui->setupUi(this);
}

DlgManageFootnotes::~DlgManageFootnotes() {
    delete ui;
}

void DlgManageFootnotes::on_pbNew_clicked() {
    DlgFootnoteEditor *dlg = new DlgFootnoteEditor(this, projectData);
    dlg->setCreateNewMode(true);
    dlg->exec();
    if(dlg->result() != QDialog::Accepted)
        return;

    Footnote *f = new Footnote(nullptr, global::getNewID(), dlg->identifier(), dlg->description());
}

