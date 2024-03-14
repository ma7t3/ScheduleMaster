#include "DlgPreferences.h"
#include "ui_DlgPreferences.h"

#include "localconfig.h"

#include <QtCore>
#include <QMessageBox>

DlgPreferences::DlgPreferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgPreferences)
{
    ui->setupUi(this);

    if(LocalConfig::language() == LocalConfig::LanguageGerman)
        ui->cbLanguage->setCurrentIndex(1);

    if(LocalConfig::compressFiles())
        ui->cbCompressFiles->setChecked(true);


}

DlgPreferences::~DlgPreferences()
{
    delete ui;
}

void DlgPreferences::on_DlgPreferences_accepted() {
    if(LocalConfig::language() != ui->cbLanguage->currentIndex())
        QMessageBox::information(this, tr("Restart required"), tr("You'll need torestarted the application for all settings to be applied"));
    
    LocalConfig::setLanguage(ui->cbLanguage->currentIndex());
    LocalConfig::setCompressFiles(ui->cbCompressFiles->isChecked());
}

