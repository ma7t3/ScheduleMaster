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
}

DlgPreferences::~DlgPreferences()
{
    delete ui;
}

void DlgPreferences::on_DlgPreferences_accepted() {
    if(LocalConfig::language() != ui->cbLanguage->currentIndex())
        QMessageBox::information(this, tr("Restart required"), tr("You'll need torestarted the application for all settings to be applied"));
    
    LocalConfig::setLanguage(ui->cbLanguage->currentIndex());
    LocalConfig::setDefaultProjectLocation(ui->leDefaultProjectLocation->text());
}


void DlgPreferences::on_pbDefaultProjectLocationBrowse_clicked() {
    QString newPath = QFileDialog::getExistingDirectory(this, "", LocalConfig::defaultProjectLocation(), QFileDialog::ShowDirsOnly);
    if(newPath.isEmpty())
        return;

    ui->leDefaultProjectLocation->setText(newPath);
}

