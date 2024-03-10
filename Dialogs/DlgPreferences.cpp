#include "DlgPreferences.h"
#include "ui_DlgPreferences.h"

#include "preferences.h"

#include <QtCore>
#include <QMessageBox>

DlgPreferences::DlgPreferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgPreferences)
{
    ui->setupUi(this);

    QSettings s("ScheduleMaster", "general");
    QString lang = s.value("language").toString();
    if(lang == "DE-de")
        ui->cbLanguage->setCurrentIndex(1);
}

DlgPreferences::~DlgPreferences()
{
    delete ui;
}

void DlgPreferences::on_DlgPreferences_accepted() {
    if(Preferences::language() != ui->cbLanguage->currentIndex())
        QMessageBox::information(this, tr("Restart required"), tr("You'll need torestarted the application for all settings to be applied"));

    Preferences::setLanguage(ui->cbLanguage->currentIndex());
}

