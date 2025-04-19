#include "WdgPreferencesPageGeneral.h"
#include "ui_WdgPreferencesPageGeneral.h"

WdgPreferencesPageGeneral::WdgPreferencesPageGeneral(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageGeneral) {
    ui->setupUi(this);

    reloadPreferences();

    connect(ui->cbLanguage, &QComboBox::currentIndexChanged, this, &WdgPreferencesPageGeneral::setUnsaved);
    connect(ui->cbLogfileMode, &QComboBox::currentIndexChanged, this, &WdgPreferencesPageGeneral::setUnsaved);
}

WdgPreferencesPageGeneral::~WdgPreferencesPageGeneral() {
    delete ui;
}

void WdgPreferencesPageGeneral::reloadPreferences() {
    ui->cbLanguage->clear();

    // available languages:
    QList<QLocale> languages = GlobalConfig::supportedLanguages();
    for(QLocale language : std::as_const(languages)) {
        ui->cbLanguage->addItem(language.nativeLanguageName(), language.name());
        if(language == LocalConfig::language())
            ui->cbLanguage->setCurrentIndex(ui->cbLanguage->count() - 1);
    }

    // logfile mode
    ui->cbLogfileMode->setCurrentIndex(LocalConfig::logfileMode());

    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageGeneral::savePreferences() {
    // language
    LocalConfig::setLanguage(ui->cbLanguage->currentData(Qt::UserRole).toString());

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

void WdgPreferencesPageGeneral::on_pbLogfileLocation_clicked() {
    qInfo() << "Opening logfile location...";
    QStringList list = LocalConfig::folderLocationPaths("base.logfile");
    if(list.isEmpty())
        return;

    QDesktopServices::openUrl(QUrl(list.first()));
}
