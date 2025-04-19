#include "WdgPreferencesPageGeneral.h"
#include "ui_WdgPreferencesPageGeneral.h"

WdgPreferencesPageGeneral::WdgPreferencesPageGeneral(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageGeneral),
    _languagesModel(new LanguagesModel(this)) {
    ui->setupUi(this);

    ui->cbLanguage->setModel(_languagesModel);

    QAction *openLogfileAction = ui->tbLogfileLocation->addAction(QIcon(":/Icons/File.ico"), tr("Open current logfile.txt"));

    openLogfileAction->setEnabled(LocalConfig::currentLogfileExists());

    connect(ui->cbLanguage,        &QComboBox::activated,           this, &WdgPreferencesPageGeneral::languageIndexChanged);

    reloadPreferences();
    connect(ui->cbLanguage,        &QComboBox::currentIndexChanged, this, &WdgPreferencesPageGeneral::setUnsaved);
    connect(ui->cbLogfileMode,     &QComboBox::currentIndexChanged, this, &WdgPreferencesPageGeneral::setUnsaved);

    connect(ui->tbLogfileLocation, &QToolButton::clicked,           this, &WdgPreferencesPageGeneral::openLogfileLocation);
    connect(openLogfileAction,   &QAction::triggered,              this, &WdgPreferencesPageGeneral::openLogfile);
}

WdgPreferencesPageGeneral::~WdgPreferencesPageGeneral() {
    delete ui;
}

void WdgPreferencesPageGeneral::reloadPreferences() {
    ui->cbLanguage->setCurrentIndex(_languagesModel->indexOfLanguage(LocalConfig::language()));

    // logfile mode
    ui->cbLogfileMode->setCurrentIndex(LocalConfig::logfileMode());

    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageGeneral::savePreferences() {
    // language
    LocalConfig::setLanguage(_languagesModel->language(ui->cbLanguage->currentIndex()));

    // logfile mode
    LocalConfig::setLogfileMode(static_cast<LocalConfig::LogfileMode>(ui->cbLogfileMode->currentIndex()));

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
    return QIcon(":/Icons/Preferences.ico");
}

void WdgPreferencesPageGeneral::setLanguageIndex(const int &index) {
    ui->cbLanguage->setCurrentIndex(index);
}

void WdgPreferencesPageGeneral::openLogfileLocation() {
    qInfo() << "Opening logfile location...";
    QStringList list = LocalConfig::folderLocationPaths("base.logfile");
    if(list.isEmpty())
        return;

    QDesktopServices::openUrl(QUrl(list.first()));
}

void WdgPreferencesPageGeneral::openLogfile() {
    qInfo() << "Opening logfile...";
    QStringList list = LocalConfig::folderLocationPaths("base.logfile");
    if(list.isEmpty())
        return;

    QDesktopServices::openUrl(QUrl(list.first() + "/logfile.txt"));
}

void WdgPreferencesPageGeneral::on_cbLogfileMode_currentIndexChanged(int index) {
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

