#include "Dialogs\DlgStartupdialog.h"
#include "ui_DlgStartupdialog.h"

#include "localconfig.h"

StartupDialog::StartupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartupDialog) {
    ui->setupUi(this);

    QStringList lastUsedFiles = LocalConfig::lastUsedFiles();

    foreach(QString path, lastUsedFiles) {
        QListWidgetItem *itm = new QListWidgetItem(ui->lwRecentFiles);
        QFileInfo fi(path);
        itm->setText(fi.fileName());
        itm->setToolTip(path);
    }
}

StartupDialog::~StartupDialog() {
    delete ui;
}

int StartupDialog::getAction() {
    return action;
}

QString StartupDialog::getFilePath() {
    return this->filePath;
}

void StartupDialog::on_commandLinkButton_clicked() {
    action = NewFile;
    accept();
}


void StartupDialog::on_commandLinkButton_2_clicked() {
    action = OpenFile;
    accept();
}


void StartupDialog::on_commandLinkButton_3_clicked() {
    action = Quit;
    accept();
}


void StartupDialog::on_lwRecentFiles_itemDoubleClicked(QListWidgetItem *item) {
    Q_UNUSED(item);
    on_pbOpenRecentFile_clicked();
}


void StartupDialog::on_pbOpenRecentFile_clicked() {
    QListWidgetItem *itm = ui->lwRecentFiles->currentItem();
    if(!itm)
        return;

    filePath = itm->toolTip();
    action = OpenRecentFile;
    accept();
}






















