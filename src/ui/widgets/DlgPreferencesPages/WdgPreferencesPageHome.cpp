#include "WdgPreferencesPageHome.h"
#include "ui_WdgPreferencesPageHome.h"

#include "src/namespace.h"
#include "src/core/LanguageServiceImpl.h"

#include "Global/StyleManager.h"
#include "Global/IconController.h"
#include "ItemModels/LanguagesModel.h"
#include "ItemModels/StylesModel.h"

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

    connect(IconController::instance(), &IconController::currentIconSetChanged, this, &WdgPreferencesPageHome::setBoxIcons);
    setBoxIcons();
}

WdgPreferencesPageHome::~WdgPreferencesPageHome() {
    delete ui;
}

void WdgPreferencesPageHome::reloadPreferences() {
    ui->cbLanguage->setCurrentIndex(_languagesModel->indexOfLanguage(SM::LanguageServiceImpl::instance()->currentLanguage()));
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
    return IconController::icon("house");
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

void WdgPreferencesPageHome::setBoxIcons() {
    ui->lTitleIconLanguage->setPixmap(IconController::icon("gear").pixmap(24, 24));
    ui->lTitleIconUpdates->setPixmap(IconController::icon("circle-rotate").pixmap(24, 24));
    ui->lTitleIconTheme->setPixmap(IconController::icon("sun-moon").pixmap(24, 24));
    ui->lTitleIconPlugins->setPixmap(IconController::icon("puzzle-piece").pixmap(24, 24));
    ui->lTitleIconKeyboardShortcuts->setPixmap(IconController::icon("keyboard").pixmap(24, 24));
    ui->lTitleIconAbout->setPixmap(IconController::icon("circle-info").pixmap(24, 24));
    ui->lTitleIconTroubleshootingDebugging->setPixmap(IconController::icon("bug").pixmap(24, 24));
}

void WdgPreferencesPageHome::on_clbOpenLogfileLocation_clicked() {
    emit openLogfileLocationRequested();
}
