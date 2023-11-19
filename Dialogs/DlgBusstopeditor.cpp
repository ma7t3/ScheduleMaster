#include "Dialogs/DlgBusstopeditor.h"
#include "ui_DlgBusstopeditor.h"

#include <QMessageBox>

#include <QCloseEvent>

busstopEditor::busstopEditor(QWidget *parent, bool m, QString n, bool i) :
    QDialog(parent),
    ui(new Ui::busstopEditor),
    createNewMode(m)
{
    ui->setupUi(this);
    ui->leName->setText(n);
    ui->cbImportant->setChecked(i);

    setCreateNewMode(m);

}
busstopEditor::~busstopEditor()
{
    delete ui;
}


void busstopEditor::setName(QString name)
{
    ui->leName->setText(name);
}

void busstopEditor::setImportant(bool important)
{
    ui->cbImportant->setChecked(important);
}

QString busstopEditor::name()
{
    return ui->leName->text();
}

bool busstopEditor::isImportant()
{
    return ui->cbImportant->isChecked();
}

void busstopEditor::setCreateNewMode(bool createNewMode)
{
    if(createNewMode)
        this->setWindowTitle(tr("Create busstop"));
}

void busstopEditor::on_busstopEditor_accepted()
{
    if(ui->leName->text() == "")
        QMessageBox::warning(this, tr("Error"), tr("No name given. Busstop was not saved!"));
}

