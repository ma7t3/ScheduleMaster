#include "WdgPreferencesPageAppearance.h"
#include "ui_WdgPreferencesPageAppearance.h"

WdgPreferencesPageAppearance::WdgPreferencesPageAppearance(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageAppearance),
    _model(new StylesModel(this)) {
    ui->setupUi(this);

    ui->cbStyle->setModel(_model);

    reloadPreferences();

    connect(ui->cbStyle,        &QComboBox::currentIndexChanged,                    this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->cssColorScheme, &WdgColorSchemeSelector::colorSchemeChanged,        this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->acsAccentColor, &WdgAccentColorSelector::currentAccentColorChanged, this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->fcbFont,        &QFontComboBox::currentFontChanged,                 this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->cbGDIEngine,    &QCheckBox::checkStateChanged,                      this, &WdgPreferencesPageAppearance::setUnsaved);

    connect(ui->cbStyle,        &QComboBox::currentIndexChanged,                    this, &WdgPreferencesPageAppearance::onStyleChanged);
    connect(ui->cssColorScheme, &WdgColorSchemeSelector::colorSchemeChanged,        this, &WdgPreferencesPageAppearance::onColorSchemeChanged);
    connect(ui->acsAccentColor, &WdgAccentColorSelector::currentAccentColorChanged, this, &WdgPreferencesPageAppearance::onAccentColorChanged);
    connect(ui->fcbFont,        &QFontComboBox::currentFontChanged,                 this, &WdgPreferencesPageAppearance::onFontChanged);

    connect(ui->cbStyle,        &QComboBox::currentIndexChanged,                    this, &WdgPreferencesPageAppearance::styleIndexChanged);
}

WdgPreferencesPageAppearance::~WdgPreferencesPageAppearance() {
    delete ui;
}

void WdgPreferencesPageAppearance::reloadPreferences() {
    ui->fcbFont->setCurrentFont(QFont(LocalConfig::uiFontFamily()));
    ui->cbGDIEngine->setChecked(LocalConfig::useGdiEngine());

    ui->cbStyle->setCurrentIndex(_model->indexOfStyle(LocalConfig::style()));
    ui->cssColorScheme->setColorScheme(LocalConfig::colorScheme());
    ui->acsAccentColor->setAccentColor(LocalConfig::accentColorID());
    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageAppearance::savePreferences() {
    LocalConfig::setUiFontFamily(ui->fcbFont->currentFont().family());
    LocalConfig::setUseGdiEngine(ui->cbGDIEngine->isChecked());

    LocalConfig::setStyle(_model->style(ui->cbStyle->currentIndex()));

    Qt::ColorScheme colorScheme = ui->cssColorScheme->colorScheme();

    Style style = StyleManager::item(_model->style(ui->cbStyle->currentIndex()));
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

void WdgPreferencesPageAppearance::setStyleIndex(const int &index) {
    ui->cbStyle->setCurrentIndex(index);
}

void WdgPreferencesPageAppearance::onStyleChanged(int index) {
    Q_UNUSED(index);
    QString id = _model->style(ui->cbStyle->currentIndex());
    StyleHandler::applyStyle(id);
    Style style = StyleManager::item(id);
    ui->flGeneral->setRowVisible(2, style.lightSupport && style.darkSupport);
    ui->flGeneral->setRowVisible(3, style.accentColorSupport);
}

void WdgPreferencesPageAppearance::onAccentColorChanged(const QString &id) {
    StyleHandler::applyAccentColor(id);
}

void WdgPreferencesPageAppearance::onColorSchemeChanged(const Qt::ColorScheme &colorScheme) {
    StyleHandler::applyColorScheme(colorScheme);
}

void WdgPreferencesPageAppearance::onFontChanged(const QFont &f) {
    StyleHandler::applyFont(f.family());
}
