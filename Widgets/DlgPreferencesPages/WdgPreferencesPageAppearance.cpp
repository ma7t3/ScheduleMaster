#include "WdgPreferencesPageAppearance.h"
#include "ui_WdgPreferencesPageAppearance.h"

#include "Global/StyleHandler.h"
#include "ItemModels/StylesModel.h"

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
    ui->fcbFont->setCurrentFont(QFont(StyleManager::currentUiFontFamily()));
    ui->cbGDIEngine->setChecked(StyleManager::gdiFontEngineEnabled());

    ui->cbStyle->setCurrentIndex(_model->indexOfStyle(StyleManager::currentStyle()));
    ui->cssColorScheme->setColorScheme(StyleManager::currentColorScheme());
    ui->acsAccentColor->setAccentColor(StyleManager::currentAccentColorID());
    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageAppearance::savePreferences() {
    StyleManager::setCurrentUiFontFamily(ui->fcbFont->currentFont().family());
    StyleManager::setGdiFontEngineEnabled(ui->cbGDIEngine->isChecked());

    StyleManager::setCurrentStyle(_model->style(ui->cbStyle->currentIndex()));

    Qt::ColorScheme colorScheme = ui->cssColorScheme->colorScheme();

    StyleConfig style = StyleManager::item(_model->style(ui->cbStyle->currentIndex()));
    if(style.supportsColorScheme(colorScheme))
        StyleManager::setCurrentColorScheme(ui->cssColorScheme->colorScheme());
    else
        StyleManager::setCurrentColorScheme(style.supportedColorSchemes().first());

    StyleManager::setCurrentAccentColor(ui->acsAccentColor->accentColorID());
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
    return QIcon(":/Icons/sun-moon.ico");
}

void WdgPreferencesPageAppearance::setStyleIndex(const int &index) {
    ui->cbStyle->setCurrentIndex(index);
}

void WdgPreferencesPageAppearance::onStyleChanged(int index) {
    Q_UNUSED(index);
    QString id = _model->style(ui->cbStyle->currentIndex());
    StyleHandler::applyStyle(id);
    StyleConfig style = StyleManager::item(id);
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
