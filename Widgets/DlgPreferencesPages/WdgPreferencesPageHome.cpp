#include "WdgPreferencesPageHome.h"
#include "ui_WdgPreferencesPageHome.h"

#include "Global/LanguageManager.h"
#include "Global/StyleManager.h"

WdgPreferencesPageHome::WdgPreferencesPageHome(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageHome),
    _languagesModel(new LanguagesModel(this)),
    _stylesModel(new StylesModel(this)) {
    ui->setupUi(this);

    ui->cbLanguage->setModel(_languagesModel);
    ui->cbStyle->setModel(_stylesModel);

    connect(ui->cbLanguage, &QComboBox::activated, this, &WdgPreferencesPageHome::languageIndexChanged);
    connect(ui->cbStyle,    &QComboBox::activated, this, &WdgPreferencesPageHome::styleIndexChanged);
    reloadPreferences();
}

WdgPreferencesPageHome::~WdgPreferencesPageHome() {
    delete ui;
}

void WdgPreferencesPageHome::reloadPreferences() {
    ui->cbLanguage->setCurrentIndex(_languagesModel->indexOfLanguage(LanguageManager::currentLanguage()));
    ui->cbStyle->setCurrentIndex(_stylesModel->indexOfStyle(StyleManager::currentStyle()));
    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageHome::savePreferences() {
    WdgPreferencesPage::savePreferences();
}

void WdgPreferencesPageHome::discardPreviewPreferences() {
    WdgPreferencesPage::discardPreviewPreferences();
}

QString WdgPreferencesPageHome::id() {
    return "base.home";
}

QString WdgPreferencesPageHome::name() {
    return tr("Home");
}

QIcon WdgPreferencesPageHome::icon() {
    return QIcon(":/Icons/Home");
}

void WdgPreferencesPageHome::setLanguageIndex(const int &index) {
    ui->cbLanguage->setCurrentIndex(index);
}

void WdgPreferencesPageHome::setStyleIndex(const int &index) {
    ui->cbStyle->setCurrentIndex(index);
}

void WdgPreferencesPageHome::on_clbKeyboardShortcuts_clicked() {
    emit openPageRequested("base.keyboardShortcuts");
}

void WdgPreferencesPageHome::on_clbConfigEditor_clicked() {
    emit openConfigEditorRequested();
}

void WdgPreferencesPageHome::on_clbUpdateSettings_clicked() {
    emit openPageRequested("base.updates");
}

void WdgPreferencesPageHome::on_clbPluginManager_clicked() {
    emit openPageRequested("base.plugins");
}

void WdgPreferencesPageHome::on_clbAppearance_clicked() {
    emit openPageRequested("base.appearance");
}

void WdgPreferencesPageHome::on_clbOpenLogfileLocation_clicked() {
    emit openLogfileLocationRequested();
}
