#include "DlgPreferences.h"
#include "ui_DlgPreferences.h"

DlgPreferences::DlgPreferences(QWidget *parent) : QDialog(parent),
    ui(new Ui::DlgPreferences) {
    ui->setupUi(this);

    for(int i = 0; i < ui->lwList->count(); i++)
        ui->lwList->item(i)->setSizeHint(QSize(0, 32));

    loadPreferences();

    ui->lwList->setFocus();
    ui->lwList->setCurrentRow(0);
}

DlgPreferences::~DlgPreferences() {
    delete ui;
}

void DlgPreferences::loadPreferences() {
    qInfo() << "Loading preferences...";

    // available languages:
    QList<QLocale::Language> languages = LocalConfig::supportedLanguages();
    for(QLocale::Language language : languages) {
        QLocale locale(language);
        ui->cbLanguage->addItem(locale.nativeLanguageName(), locale.name());
        if(language == LocalConfig::language())
            ui->cbLanguage->setCurrentIndex(ui->cbLanguage->count() - 1);
    }

    ui->cbLogfileMode->setCurrentIndex(LocalConfig::logfileMode());
}

void DlgPreferences::savePreferences() {
    qInfo() << "Saving preferences...";
}

void DlgPreferences::on_lwList_currentItemChanged(QListWidgetItem *current,
                                                  QListWidgetItem *previous) {
    Q_UNUSED(previous);
    ui->swContent->setCurrentIndex(ui->lwList->row(current));
    ui->lTitle->setText(current->text());
    ui->lTitleIcon->setPixmap(current->icon().pixmap(28, 28));
}

void DlgPreferences::on_pbReset_clicked() {
    QMessageBox::warning(this, tr("Not available"), tr("Reset preferences ist not available now."));
}

void DlgPreferences::accept() {
    savePreferences();
    QDialog::accept();
}

void DlgPreferences::on_cbLogfileMode_currentIndexChanged(int index) {
    QString infoText;

    switch(index) {
    case LocalConfig::LogfileMode::NoLog:
        infoText = tr("No log file will be created.");
        break;
    case LocalConfig::LogfileMode::DefaultLog:
        infoText = tr("A normal logfile is generated, containing normal infos, warnings and error messages.");
        break;
    case LocalConfig::LogfileMode::DebugLog:
        infoText = tr("An advanced log file will be created including additional debug information. Helpful for developers or testers.");
        break;
    case LocalConfig::LogfileMode::DebugDetailLog:
        infoText = tr("An advanced log file will be created including additional debug information. Helpful for developers or testers. Also including exact code file names and line numers where the message was sent. Only works in debug builds.");
        break;
    }

    ui->lLogfileModeInfo->setText(infoText);
}
