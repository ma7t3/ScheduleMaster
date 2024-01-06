#include "DlgFootnoteEditor.h"
#include "ui_DlgFootnoteEditor.h"

DlgFootnoteEditor::DlgFootnoteEditor(QWidget *parent, ProjectData *projectData) :
    QDialog(parent), ui(new Ui::DlgFootnoteEditor),
    projectData(projectData) {
    ui->setupUi(this);
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

QString DlgFootnoteEditor::description() const {
    return ui->leDescription->text();
}

void DlgFootnoteEditor::setDescription(const QString &newDescription) {
    ui->leDescription->setText(newDescription);
}
