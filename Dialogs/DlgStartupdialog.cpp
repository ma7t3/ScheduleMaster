#include "Dialogs\DlgStartupdialog.h"
#include "ui_DlgStartupdialog.h"

StartupDialog::StartupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartupDialog)
{
    ui->setupUi(this);
}

StartupDialog::~StartupDialog()
{
    delete ui;
}

int StartupDialog::getAction()
{
    return action;
}

QString StartupDialog::getFilePath()
{
    return this->filePath;
}

void StartupDialog::on_commandLinkButton_clicked()
{
    action = newFile;
    accept();
}


void StartupDialog::on_commandLinkButton_2_clicked()
{
    action = openFile;
    accept();
}


void StartupDialog::on_commandLinkButton_3_clicked()
{
    action = quit;
    accept();
}


void StartupDialog::on_lwRecentFiles_itemDoubleClicked(QListWidgetItem *item)
{
    on_pbOpenRecentFile_clicked();
}


void StartupDialog::on_pbOpenRecentFile_clicked()
{
    QListWidgetItem *itm = ui->lwRecentFiles->currentItem();
    if(!itm)
        return;

    filePath = itm->toolTip();
    action = openRecentFile;
    accept();
}






















