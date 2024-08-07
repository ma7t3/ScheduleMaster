#include "Dialogs/DlgBusstopeditor.h"
#include "ui_DlgBusstopeditor.h"

#include <QMessageBox>

#include <QCloseEvent>

busstopEditor::busstopEditor(QWidget *parent, Busstop busstop, bool createMode) :
    QDialog(parent),
    ui(new Ui::busstopEditor),
    _busstop(busstop) {
    ui->setupUi(this);

    setCreateNewMode(createMode);
    loadBusstop();
    ui->leName->setFocus();
}
busstopEditor::~busstopEditor() {
    delete ui;
}

void busstopEditor::setCreateNewMode(const bool &createNewMode) {
    if(createNewMode)
        setWindowTitle(tr("Create busstop"));
}

Busstop busstopEditor::busstop() const {
    Busstop b = _busstop;
    b.setName(ui->leName->text());
    b.setImportant(ui->cbImportant->isChecked());
    return b;
}

void busstopEditor::loadBusstop() {
    ui->leName->setText(_busstop.name());
    ui->cbImportant->setChecked(_busstop.isImportant());
}
