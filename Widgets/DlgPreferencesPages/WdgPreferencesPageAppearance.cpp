#include "WdgPreferencesPageAppearance.h"
#include "ui_WdgPreferencesPageAppearance.h"

WdgPreferencesPageAppearance::WdgPreferencesPageAppearance(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageAppearance) {
    ui->setupUi(this);

    reloadPreferences();

    connect(ui->cbAppearance, &QComboBox::currentIndexChanged, this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->fcbFont, &QFontComboBox::currentFontChanged, this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->cbGDIEngine, &QCheckBox::checkStateChanged, this, &WdgPreferencesPageAppearance::setUnsaved);
}

WdgPreferencesPageAppearance::~WdgPreferencesPageAppearance() {
    delete ui;
}

void WdgPreferencesPageAppearance::reloadPreferences() {
    ui->fcbFont->setCurrentFont(QFont(LocalConfig::uiFontFamily()));
    ui->cbGDIEngine->setChecked(LocalConfig::useGdiEngine());
    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageAppearance::savePreferences() {
    LocalConfig::setUiFontFamily(ui->fcbFont->currentFont().family());
    LocalConfig::setUseGdiEngine(ui->cbGDIEngine->isChecked());
    WdgPreferencesPage::savePreferences();
}

void WdgPreferencesPageAppearance::discardPreviewPreferences() {
    LocalConfig::restoreUiFontFamilyPreview();
}

QString WdgPreferencesPageAppearance::id() {
    return "base.appearance";
}

QString WdgPreferencesPageAppearance::name() {
    return tr("Appearance");
}

QIcon WdgPreferencesPageAppearance::icon() {
    return QIcon(":/Icons/Appearance.ico");
}

void WdgPreferencesPageAppearance::on_fcbFont_currentFontChanged(const QFont &f) {
    LocalConfig::previewUiFontFamily(f.family());
}
