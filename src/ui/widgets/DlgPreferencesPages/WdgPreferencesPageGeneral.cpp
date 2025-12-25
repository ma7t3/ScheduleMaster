#include "WdgPreferencesPageGeneral.h"
#include "ui_WdgPreferencesPageGeneral.h"

#include "src/core/LoggerImpl.h"
#include "src/core/LanguageServiceImpl.h"
#include "src/core/FolderLocationServiceImpl.h"

#include "Global/ActionController.h"
#include "Global/IconController.h"
#include "ItemModels/LanguagesModel.h"

#include <QDesktopServices>

WdgPreferencesPageGeneral::WdgPreferencesPageGeneral(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageGeneral),
    _languagesModel(new LanguagesModel(this)), _logger(static_cast<SM::LoggerImpl *>(SM::LoggerImpl::instance())) {
    ui->setupUi(this);

    ui->cbLanguage->setModel(_languagesModel);

    QAction *openLogfileAction = ui->tbLogfileLocation->addAction("");
    openLogfileAction->setEnabled(_logger->currentLogfileExists());
    connect(ui->tbLogfileLocation, &QToolButton::clicked,           this, &WdgPreferencesPageGeneral::openLogfileLocation);
    connect(openLogfileAction,     &QAction::triggered,             this, &WdgPreferencesPageGeneral::openLogfile);

    reloadPreferences();

    connect(ui->cbLanguage,        &QComboBox::activated,           this, &WdgPreferencesPageGeneral::languageIndexChanged);

    connect(ui->cbLanguage,        &QComboBox::currentIndexChanged, this, &WdgPreferencesPageGeneral::setUnsaved);
    connect(ui->cbLogfileMode,     &QComboBox::currentIndexChanged, this, &WdgPreferencesPageGeneral::setUnsaved);

    ActionController::add(ui->tbLogfileLocation, "application.preferences.general.openLogfileLocation");
    ActionController::add(openLogfileAction,     "application.preferences.general.openCurrentLogfile");
}

WdgPreferencesPageGeneral::~WdgPreferencesPageGeneral() {
    delete ui;
}

void WdgPreferencesPageGeneral::reloadPreferences() {
    ui->cbLanguage->setCurrentIndex(_languagesModel->indexOfLanguage(SM::LanguageServiceImpl::instance()->currentLanguage()));

    // logfile mode
    ui->cbLogfileMode->setCurrentIndex(_logger->logfileMode());

    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageGeneral::savePreferences() {
    // language
    SM::LanguageServiceImpl::instance()->setCurrentLanguage(_languagesModel->language(ui->cbLanguage->currentIndex()).locale.name());

    // logfile mode
    _logger->setLogfileMode(static_cast<SM::LoggerImpl::LogfileMode>(ui->cbLogfileMode->currentIndex()));

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
    return IconController::icon("gear");
}

void WdgPreferencesPageGeneral::setLanguageIndex(const int &index) {
    ui->cbLanguage->setCurrentIndex(index);
}

void WdgPreferencesPageGeneral::openLogfileLocation() {
    qInfo() << "Opening logfile location...";
    QStringList list = SM::FolderLocationServiceImpl::instance()->currentFolderLocationPaths("logfile");
    if(list.isEmpty())
        return;

    QDesktopServices::openUrl(QUrl(list.first()));
}

void WdgPreferencesPageGeneral::openLogfile() {
    qInfo() << "Opening logfile...";
    QStringList list = SM::FolderLocationServiceImpl::instance()->currentFolderLocationPaths("logfile");
    if(list.isEmpty())
        return;

    QDesktopServices::openUrl(QUrl(list.first() + "/logfile.txt"));
}

void WdgPreferencesPageGeneral::on_cbLogfileMode_currentIndexChanged(int index) {
    QString infoText;

    switch(index) {
    case SM::LoggerImpl::NoLog:
        infoText = tr("No log file will be created.");
        break;
    case SM::LoggerImpl::DefaultLog:
        infoText = tr("A normal logfile is generated, containing normal infos, warnings and error messages.");
        break;
    case SM::LoggerImpl::DebugLog:
        infoText = tr("An advanced log file will be created including additional debug information. Helpful for developers or testers.");
        break;
    case SM::LoggerImpl::DebugDetailLog:
        infoText = tr("An advanced log file will be created including additional debug information. Helpful for developers or testers. Also including exact code file names and line numers where the message was sent. Only works in debug builds.");
        break;
    }

    ui->lLogfileModeInfo->setText(infoText);
}

