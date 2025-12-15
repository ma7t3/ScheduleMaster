#include "WdgPreferencesPageAppearance.h"
#include "ui_WdgPreferencesPageAppearance.h"

#include "Global/SettingsManager.h"
#include "Global/StyleHandler.h"
#include "ItemModels/StylesModel.h"
#include "ItemModels/IconSetsModel.h"
#include "Global/IconController.h"

WdgPreferencesPageAppearance::WdgPreferencesPageAppearance(QWidget *parent) :
    WdgPreferencesPage(parent),
    ui(new Ui::WdgPreferencesPageAppearance),
    _stylesModel(new StylesModel(this)),
    _iconSetsModel(new IconSetsModel(this)) {
    ui->setupUi(this);

    ui->cbStyle->setModel(_stylesModel);
    ui->cbIconSet->setModel(_iconSetsModel);

    reloadPreferences();

    connect(ui->cbStyle,        &QComboBox::currentIndexChanged,                    this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->cbIconSet,      &QComboBox::currentIndexChanged,                    this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->cssColorScheme, &WdgColorSchemeSelector::colorSchemeChanged,        this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->acsAccentColor, &WdgAccentColorSelector::currentAccentColorChanged, this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->fcbFont,        &QFontComboBox::currentFontChanged,                 this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->cbGDIEngine,    &QCheckBox::checkStateChanged,                      this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->hsUiScale,      &QSlider::valueChanged,                             this, &WdgPreferencesPageAppearance::setUnsaved);
    connect(ui->sbUiScale,      &QSpinBox::valueChanged,                            this, &WdgPreferencesPageAppearance::setUnsaved);

    connect(ui->cbStyle,        &QComboBox::currentIndexChanged,                    this, &WdgPreferencesPageAppearance::onStyleChanged);
    connect(ui->cbIconSet,      &QComboBox::currentIndexChanged,                    this, &WdgPreferencesPageAppearance::onIconSetChanged);
    connect(ui->cssColorScheme, &WdgColorSchemeSelector::colorSchemeChanged,        this, &WdgPreferencesPageAppearance::onColorSchemeChanged);
    connect(ui->acsAccentColor, &WdgAccentColorSelector::currentAccentColorChanged, this, &WdgPreferencesPageAppearance::onAccentColorChanged);
    connect(ui->fcbFont,        &QFontComboBox::currentFontChanged,                 this, &WdgPreferencesPageAppearance::onFontChanged);

    connect(ui->cbStyle,        &QComboBox::currentIndexChanged,                    this, &WdgPreferencesPageAppearance::styleIndexChanged);

    connect(ui->hsUiScale,      &QSlider::valueChanged,                             ui->sbUiScale, &QSpinBox::setValue);
    connect(ui->sbUiScale,      &QSpinBox::valueChanged,                            ui->hsUiScale, &QSlider::setValue);

    onStyleChanged(ui->cbStyle->currentIndex());;
}

WdgPreferencesPageAppearance::~WdgPreferencesPageAppearance() {
    delete ui;
}

void WdgPreferencesPageAppearance::reloadPreferences() {
    ui->fcbFont->setCurrentFont(QFont(StyleManager::currentUiFontFamily()));
    ui->cbGDIEngine->setChecked(StyleManager::gdiFontEngineEnabled());

    ui->cbStyle->setCurrentIndex(_stylesModel->indexOfStyle(StyleManager::currentStyle()));
    ui->cbIconSet->setCurrentIndex(_iconSetsModel->indexOficonSet(IconSetManager::currentIconSet()));
    ui->cssColorScheme->setColorScheme(StyleManager::currentColorScheme());
    ui->acsAccentColor->setAccentColor(StyleManager::currentAccentColorID());
    ui->sbUiScale->setValue(SettingsManager::value("appearance.uiScale").toFloat() * 100);
    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageAppearance::savePreferences() {
    StyleManager::setCurrentUiFontFamily(ui->fcbFont->currentFont().family());
    StyleManager::setGdiFontEngineEnabled(ui->cbGDIEngine->isChecked());

    StyleManager::setCurrentStyle(_stylesModel->style(ui->cbStyle->currentIndex()));
    IconSetManager::setCurrentIconSet(_iconSetsModel->iconSet(ui->cbIconSet->currentIndex()));

    Qt::ColorScheme colorScheme = ui->cssColorScheme->colorScheme();

    StyleConfig style = StyleManager::item(_stylesModel->style(ui->cbStyle->currentIndex()));
    if(style.supportsColorScheme(colorScheme))
        StyleManager::setCurrentColorScheme(ui->cssColorScheme->colorScheme());
    else
        StyleManager::setCurrentColorScheme(style.supportedColorSchemes().first());

    StyleManager::setCurrentAccentColor(ui->acsAccentColor->accentColorID());
    SettingsManager::setValue("appearance.uiScale", ui->sbUiScale->value() / 100.0);
    WdgPreferencesPage::savePreferences();
}

void WdgPreferencesPageAppearance::discardPreviewPreferences() {
    StyleHandler::applyFont();
    StyleHandler::applyStyle();
    IconController::applyIconSet();
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
    return IconController::icon("sun-moon");
}

void WdgPreferencesPageAppearance::setStyleIndex(const int &index) {
    ui->cbStyle->setCurrentIndex(index);
}

void WdgPreferencesPageAppearance::onIconSetChanged(const int &index) {
    QString id = _iconSetsModel->iconSet(ui->cbIconSet->currentIndex());
    IconController::applyIconSet(id);
}

void WdgPreferencesPageAppearance::onStyleChanged(const int &index) {
    Q_UNUSED(index);
    QString id = _stylesModel->style(ui->cbStyle->currentIndex());
    StyleHandler::applyStyle(id);
    StyleConfig style = StyleManager::item(id);
    ui->flGeneral->setRowVisible(3, style.lightSupport && style.darkSupport);
    ui->flGeneral->setRowVisible(4, style.accentColorSupport);
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
