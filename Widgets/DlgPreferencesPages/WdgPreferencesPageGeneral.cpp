#include "WdgPreferencesPageGeneral.h"
#include "ui_WdgPreferencesPageGeneral.h"

#include "Global/LanguageManager.h"
#include "Global/Logger.h"
#include "Global/FolderLocationManager.h"
#include "ItemModels/LanguagesModel.h"

#include <QDesktopServices>

WdgPreferencesPageGeneral::WdgPreferencesPageGeneral(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageGeneral),
    _languagesModel(new LanguagesModel(this)) {
    ui->setupUi(this);

    ui->cbLanguage->setModel(_languagesModel);

    QAction *openLogfileAction = ui->tbLogfileLocation->addAction(QIcon(":/Icons/classic/file.ico"), tr("Open current logfile.txt"));
    openLogfileAction->setEnabled(Logger::currentLogfileExists());
    connect(ui->tbLogfileLocation, &QToolButton::clicked,           this, &WdgPreferencesPageGeneral::openLogfileLocation);
    connect(openLogfileAction,     &QAction::triggered,             this, &WdgPreferencesPageGeneral::openLogfile);

    reloadPreferences();

    connect(ui->cbLanguage,        &QComboBox::activated,           this, &WdgPreferencesPageGeneral::languageIndexChanged);

    connect(ui->cbLanguage,        &QComboBox::currentIndexChanged, this, &WdgPreferencesPageGeneral::setUnsaved);
    connect(ui->cbLogfileMode,     &QComboBox::currentIndexChanged, this, &WdgPreferencesPageGeneral::setUnsaved);

}

WdgPreferencesPageGeneral::~WdgPreferencesPageGeneral() {
    delete ui;
}

void WdgPreferencesPageGeneral::reloadPreferences() {
    ui->cbLanguage->setCurrentIndex(_languagesModel->indexOfLanguage(LanguageManager::currentLanguage()));

    // logfile mode
    ui->cbLogfileMode->setCurrentIndex(Logger::logfileMode());

    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageGeneral::savePreferences() {
    // language
    LanguageManager::setCurrentLanguage(_languagesModel->language(ui->cbLanguage->currentIndex()).locale.name());

    // logfile mode
    Logger::setLogfileMode(static_cast<Logger::LogfileMode>(ui->cbLogfileMode->currentIndex()));

    WdgPreferencesPage::savePreferences();
}

void WdgPreferencesPageGeneral::discardPreviewPreferences() {
    WdgPreferencesPage::discardPreviewPreferences();
}

QString WdgPreferencesPageGeneral::id() {
    return "base.general";
}

QString WdgPreferencesPageGeneral::name() {
    return tr("General");
}

QIcon WdgPreferencesPageGeneral::icon() {
    return QIcon(":/Icons/classic/gear.ico");
}

void WdgPreferencesPageGeneral::setLanguageIndex(const int &index) {
    ui->cbLanguage->setCurrentIndex(index);
}

void WdgPreferencesPageGeneral::openLogfileLocation() {
    qInfo() << "Opening logfile location...";
    QStringList list = FolderLocationManager::currentFolderLocationPaths("logfile");
    if(list.isEmpty())
        return;

    QDesktopServices::openUrl(QUrl(list.first()));
}

void WdgPreferencesPageGeneral::openLogfile() {
    qInfo() << "Opening logfile...";
    QStringList list = FolderLocationManager::currentFolderLocationPaths("logfile");
    if(list.isEmpty())
        return;

    QDesktopServices::openUrl(QUrl(list.first() + "/logfile.txt"));
}

void WdgPreferencesPageGeneral::on_cbLogfileMode_currentIndexChanged(int index) {
    QString infoText;

    switch(index) {
    case Logger::NoLog:
        infoText = tr("No log file will be created.");
        break;
    case Logger::DefaultLog:
        infoText = tr("A normal logfile is generated, containing normal infos, warnings and error messages.");
        break;
    case Logger::DebugLog:
        infoText = tr("An advanced log file will be created including additional debug information. Helpful for developers or testers.");
        break;
    case Logger::DebugDetailLog:
        infoText = tr("An advanced log file will be created including additional debug information. Helpful for developers or testers. Also including exact code file names and line numers where the message was sent. Only works in debug builds.");
        break;
    }

    ui->lLogfileModeInfo->setText(infoText);
}

