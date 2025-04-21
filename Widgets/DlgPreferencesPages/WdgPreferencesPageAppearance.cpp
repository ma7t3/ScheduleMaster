#include "WdgPreferencesPageAppearance.h"
#include "ui_WdgPreferencesPageAppearance.h"

WdgPreferencesPageAppearance::WdgPreferencesPageAppearance(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageAppearance) {
    ui->setupUi(this);

    ui->cbAppearance->clear();
    ui->cbAppearance->addItems(GlobalConfig::styles().keys());

    reloadPreferences();

    connect(ui->cbAppearance,   &QComboBox::currentIndexChanged,                    this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->cssColorScheme, &WdgColorSchemeSelector::colorSchemeChanged,        this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->acsAccentColor, &WdgAccentColorSelector::currentAccentColorChanged, this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->fcbFont,        &QFontComboBox::currentFontChanged,                 this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->cbGDIEngine,    &QCheckBox::checkStateChanged,                      this, &WdgPreferencesPageAppearance::setUnsaved);

    connect(ui->acsAccentColor, &WdgAccentColorSelector::currentAccentColorChanged, this, &WdgPreferencesPageAppearance::onAccentColorChanged);
    connect(ui->cssColorScheme, &WdgColorSchemeSelector::colorSchemeChanged,        this, &WdgPreferencesPageAppearance::onColorSchemeChanged);
}

WdgPreferencesPageAppearance::~WdgPreferencesPageAppearance() {
    delete ui;
}

void WdgPreferencesPageAppearance::reloadPreferences() {
    ui->fcbFont->setCurrentFont(QFont(LocalConfig::uiFontFamily()));
    ui->cbGDIEngine->setChecked(LocalConfig::useGdiEngine());

    // TODO: Load style
    ui->cbAppearance->setCurrentText(LocalConfig::style());

    ui->cssColorScheme->setColorScheme(LocalConfig::colorScheme());
    ui->acsAccentColor->setAccentColor(LocalConfig::accentColorID());
    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageAppearance::savePreferences() {
    LocalConfig::setUiFontFamily(ui->fcbFont->currentFont().family());
    LocalConfig::setUseGdiEngine(ui->cbGDIEngine->isChecked());
    // TODO: LocalConfig::setStyle();
    LocalConfig::setStyle(ui->cbAppearance->currentText());

    Qt::ColorScheme colorScheme = ui->cssColorScheme->colorScheme();

    Style style = GlobalConfig::style(ui->cbAppearance->currentText());
    if(style.supportsColorScheme(colorScheme))
        LocalConfig::setColorScheme(ui->cssColorScheme->colorScheme());
    else
        LocalConfig::setColorScheme(style.supportedColorSchemes().first());

    LocalConfig::setAccentColor(ui->acsAccentColor->accentColorID());
    WdgPreferencesPage::savePreferences();
}

void WdgPreferencesPageAppearance::discardPreviewPreferences() {
    StyleHandler::applyFont();
    StyleHandler::applyStyle();
    StyleHandler::applyAccentColor();
    StyleHandler::applyColorScheme();
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

void WdgPreferencesPageAppearance::onColorSchemeChanged(const Qt::ColorScheme &colorScheme) {
    qDebug() << colorScheme;
    StyleHandler::applyColorScheme(colorScheme);
}

void WdgPreferencesPageAppearance::on_cbAppearance_currentIndexChanged(int index) {
    QString id = ui->cbAppearance->currentText();
    StyleHandler::applyStyle(id);
    Style style = GlobalConfig::style(id);
    ui->flGeneral->setRowVisible(2, style.lightSupport && style.darkSupport);
    ui->flGeneral->setRowVisible(3, style.accentColorSupport);
}
