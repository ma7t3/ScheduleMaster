#include "WdgPreferencesPageAppearance.h"
#include "ui_WdgPreferencesPageAppearance.h"

WdgPreferencesPageAppearance::WdgPreferencesPageAppearance(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageAppearance) {
    ui->setupUi(this);

    ui->cbAppearance->clear();
    ui->cbAppearance->addItems(GlobalConfig::styles().keys());

    reloadPreferences();

    connect(ui->cbAppearance, &QComboBox::currentIndexChanged, this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->fcbFont, &QFontComboBox::currentFontChanged, this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->cbGDIEngine, &QCheckBox::checkStateChanged, this, &WdgPreferencesPageAppearance::setUnsaved);

    connect(ui->wdgAccentColor, &WdgAccentColorSelector::onCurrentAccentColorChanged, this, &WdgPreferencesPageAppearance::onAccentColorChanged);
    connect(ui->wdgAccentColor, &WdgAccentColorSelector::onCurrentAccentColorChanged, this, &WdgPreferencesPageAppearance::setUnsaved);
}

WdgPreferencesPageAppearance::~WdgPreferencesPageAppearance() {
    delete ui;
}

void WdgPreferencesPageAppearance::reloadPreferences() {
    ui->fcbFont->setCurrentFont(QFont(LocalConfig::uiFontFamily()));
    ui->cbGDIEngine->setChecked(LocalConfig::useGdiEngine());

    // TODO: Load style
    ui->cbAppearance->setCurrentText(LocalConfig::style());

    ui->wdgAccentColor->setAccentColor(LocalConfig::accentColorID());
    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageAppearance::savePreferences() {
    LocalConfig::setUiFontFamily(ui->fcbFont->currentFont().family());
    LocalConfig::setUseGdiEngine(ui->cbGDIEngine->isChecked());
    // TODO: LocalConfig::setStyle();
    LocalConfig::setStyle(ui->cbAppearance->currentText());
    LocalConfig::setAccentColor(ui->wdgAccentColor->accentColorID());
    WdgPreferencesPage::savePreferences();
}

void WdgPreferencesPageAppearance::discardPreviewPreferences() {
    StyleHandler::applyFont();
    StyleHandler::applyStyle();
    StyleHandler::applyAccentColor();
    WdgPreferencesPage::discardPreviewPreferences();
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
    StyleHandler::applyFont(f.family());
}

void WdgPreferencesPageAppearance::onAccentColorChanged(const QString &id) {
    StyleHandler::applyAccentColor(id);
}

void WdgPreferencesPageAppearance::on_cbAppearance_activated(int index) {
    QString id = ui->cbAppearance->currentText();
    StyleHandler::applyStyle(id);
}
