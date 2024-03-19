#include "DlgPreferences.h"
#include "ui_DlgPreferences.h"

#include "localconfig.h"

#include <QtCore>
#include <QMessageBox>
#include <QFileDialog>

DlgPreferences::DlgPreferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgPreferences)
{
    ui->setupUi(this);

    if(LocalConfig::language() == LocalConfig::LanguageGerman)
        ui->cbLanguage->setCurrentIndex(1);

    ui->leDefaultProjectLocation->setText(LocalConfig::defaultProjectLocation());
    ui->cbLogfileMode->setCurrentIndex(LocalConfig::logfileMode());
}

DlgPreferences::~DlgPreferences()
{
    delete ui;
}

void DlgPreferences::on_DlgPreferences_accepted() {
    int logfileMode = ui->cbLogfileMode->currentIndex();

    if(LocalConfig::language() != ui->cbLanguage->currentIndex() ||
        LocalConfig::logfileMode() != logfileMode)
        QMessageBox::information(this, tr("Restart required"), tr("You'll need torestarted the application for all settings to be applied"));
    
    LocalConfig::setLanguage(ui->cbLanguage->currentIndex());
    LocalConfig::setDefaultProjectLocation(ui->leDefaultProjectLocation->text());

    if(logfileMode == LocalConfig::NoLog)
        LocalConfig::setLogfileMode(LocalConfig::NoLog);
    if(logfileMode == LocalConfig::DefaultLog)
        LocalConfig::setLogfileMode(LocalConfig::DefaultLog);
    if(logfileMode == LocalConfig::DebugLog)
        LocalConfig::setLogfileMode(LocalConfig::DebugLog);
    if(logfileMode == LocalConfig::DebugDetailLog)
        LocalConfig::setLogfileMode(LocalConfig::DebugDetailLog);
}


void DlgPreferences::on_pbDefaultProjectLocationBrowse_clicked() {
    QString newPath = QFileDialog::getExistingDirectory(this, "", LocalConfig::defaultProjectLocation(), QFileDialog::ShowDirsOnly);
    if(newPath.isEmpty())
        return;

    ui->leDefaultProjectLocation->setText(newPath);
}

