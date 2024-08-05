#include "DlgPreferences.h"
#include "ui_DlgPreferences.h"

#include "localconfig.h"

#include <QtCore>
#include <QMessageBox>
#include <QFileDialog>

#include <QStyleFactory>

DlgPreferences::DlgPreferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgPreferences)
{
    ui->setupUi(this);

    if(LocalConfig::language() == LocalConfig::German)
        ui->cbLanguage->setCurrentIndex(1);

    ui->cbTimeFormat->setCurrentIndex(LocalConfig::timeFormat());

    if(LocalConfig::style() == LocalConfig::Fusion)
        ui->cbApperance->setCurrentIndex(2);
    else if(LocalConfig::style() == LocalConfig::WindowsXpStyle)
        ui->cbApperance->setCurrentIndex(1);
    else
        ui->cbApperance->setCurrentIndex(0);

    ui->leDefaultProjectLocation->setText(LocalConfig::defaultProjectLocation());
    ui->cbLogfileMode->setCurrentIndex(LocalConfig::logfileMode());

    switch(LocalConfig::workspacesToolbarPosition()) {
        default:
        case Qt::TopToolBarArea:    ui->cbWorkspacesToolbarPosition->setCurrentIndex(0); break;
        case Qt::BottomToolBarArea: ui->cbWorkspacesToolbarPosition->setCurrentIndex(1); break;
        case Qt::LeftToolBarArea:   ui->cbWorkspacesToolbarPosition->setCurrentIndex(2); break;
        case Qt::RightToolBarArea:  ui->cbWorkspacesToolbarPosition->setCurrentIndex(3); break;
    }

    connect(ui->cbApperance, &QComboBox::currentIndexChanged, this, &DlgPreferences::refreshStylePreview);
}

DlgPreferences::~DlgPreferences()
{
    delete ui;
}

void DlgPreferences::on_DlgPreferences_accepted() {
    int logfileMode = ui->cbLogfileMode->currentIndex();

    if(LocalConfig::language() != ui->cbLanguage->currentIndex() ||
        LocalConfig::logfileMode() != logfileMode)
        QMessageBox::information(this, tr("Restart required"), tr("You'll need to restarted the application for all settings to be applied."));
    
    LocalConfig::setLanguage(ui->cbLanguage->currentIndex() == 1 ? LocalConfig::German : LocalConfig::English);
    LocalConfig::setTimeFormat(ui->cbTimeFormat->currentIndex() == 0 ? LocalConfig::Hours12 : LocalConfig::Hours24);
    LocalConfig::setDefaultProjectLocation(ui->leDefaultProjectLocation->text());

    if(ui->cbApperance->currentIndex() == 2)
        LocalConfig::setStyle(LocalConfig::Fusion);
    else if(ui->cbApperance->currentIndex() == 1)
        LocalConfig::setStyle(LocalConfig::WindowsXpStyle);
    else
        LocalConfig::setStyle(LocalConfig::System);

    if(logfileMode == LocalConfig::NoLog)
        LocalConfig::setLogfileMode(LocalConfig::NoLog);
    if(logfileMode == LocalConfig::DefaultLog)
        LocalConfig::setLogfileMode(LocalConfig::DefaultLog);
    if(logfileMode == LocalConfig::DebugLog)
        LocalConfig::setLogfileMode(LocalConfig::DebugLog);
    if(logfileMode == LocalConfig::DebugDetailLog)
        LocalConfig::setLogfileMode(LocalConfig::DebugDetailLog);

    switch(ui->cbWorkspacesToolbarPosition->currentIndex()) {
        case 0: LocalConfig::setWorkspacesToolbarPosition(Qt::TopToolBarArea)   ; break;
        case 1: LocalConfig::setWorkspacesToolbarPosition(Qt::BottomToolBarArea); break;
        case 2: LocalConfig::setWorkspacesToolbarPosition(Qt::LeftToolBarArea)  ; break;
        case 3: LocalConfig::setWorkspacesToolbarPosition(Qt::RightToolBarArea) ; break;
    }

}

void DlgPreferences::on_DlgPreferences_rejected() {
    if(LocalConfig::style() == LocalConfig::Fusion)
        qApp->setStyle(QStyleFactory::create("Fusion"));
    else if(LocalConfig::style() == LocalConfig::WindowsXpStyle)
        qApp->setStyle(QStyleFactory::create("Windows"));
    else
        qApp->setStyle(QStyleFactory::create("windowsvista"));
}

void DlgPreferences::on_pbDefaultProjectLocationBrowse_clicked() {
    QString newPath = QFileDialog::getExistingDirectory(this, "", LocalConfig::defaultProjectLocation(), QFileDialog::ShowDirsOnly);
    if(newPath.isEmpty())
        return;

    ui->leDefaultProjectLocation->setText(newPath);
}

void DlgPreferences::refreshStylePreview(int index) {
    if(index == 0)
        qApp->setStyle(QStyleFactory::create("windowsvista"));
    else if(index == 1)
        qApp->setStyle(QStyleFactory::create("Windows"));
    else if(index == 2)
        qApp->setStyle(QStyleFactory::create("Fusion"));
}

