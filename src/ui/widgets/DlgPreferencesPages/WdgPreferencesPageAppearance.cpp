#include "WdgPreferencesPageAppearance.h"
#include "ui_WdgPreferencesPageAppearance.h"

#include "src/namespace.h"
#include "src/core/SettingsServiceImpl.h"
#include "src/core/IconServiceImpl.h"
#include "src/core/AppearanceServiceImpl.h"

#include "ItemModels/StylesModel.h"
#include "ItemModels/IconSetsModel.h"

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
    static auto appearanceService = SM::AppearanceServiceImpl::instance();
    ui->fcbFont->setCurrentFont(QFont(appearanceService->currentUiFontFamily()));
    ui->cbGDIEngine->setChecked(SM::SettingsServiceImpl::instance()->value("appearance.fontEngineGDI").toBool());

    ui->cbStyle->setCurrentIndex(_stylesModel->indexOfStyle(appearanceService->currentStyleID()));
    ui->cbIconSet->setCurrentIndex(_iconSetsModel->indexOficonSet(SM::IconServiceImpl::instance()->currentIconSet()));
    ui->cssColorScheme->setColorScheme(appearanceService->currentColorScheme());
    ui->acsAccentColor->setAccentColor(appearanceService->currentAccentColorID());
    ui->sbUiScale->setValue(SM::SettingsServiceImpl::instance()->value("appearance.uiScale").toFloat() * 100);
    WdgPreferencesPage::reloadPreferences();
}

void WdgPreferencesPageAppearance::savePreferences() {
    static auto appearanceService = SM::AppearanceServiceImpl::instance();
    appearanceService->setCurrentUiFontFamily(ui->fcbFont->currentFont().family());
    SM::SettingsServiceImpl::instance()->setValue("appearance.fontEngineGDI", ui->cbGDIEngine->isChecked());

    appearanceService->setCurrentStyle(_stylesModel->style(ui->cbStyle->currentIndex()));
    SM::IconServiceImpl::instance()->setCurrentIconSet(_iconSetsModel->iconSet(ui->cbIconSet->currentIndex()));

    Qt::ColorScheme colorScheme = ui->cssColorScheme->colorScheme();

    SMA::StyleConfig style = appearanceService->style(_stylesModel->style(ui->cbStyle->currentIndex()));
    if(style.supportsColorScheme(colorScheme))
        appearanceService->setCurrentColorScheme(ui->cssColorScheme->colorScheme());
    else
        appearanceService->setCurrentColorScheme(style.supportedColorSchemes().first());

    appearanceService->setCurrentAccentColor(ui->acsAccentColor->accentColorID());
    SM::SettingsServiceImpl::instance()->setValue("appearance.uiScale", ui->sbUiScale->value() / 100.0);
    WdgPreferencesPage::savePreferences();
}

void WdgPreferencesPageAppearance::discardPreviewPreferences() {
    static auto appearanceService = SM::AppearanceServiceImpl::instance();
    appearanceService->endStylePreview();
    appearanceService->endColorSchemePreview();
    appearanceService->endAccentColorPreview();
    appearanceService->endUiFontFamilyPreview();
    SM::IconServiceImpl::instance()->discardIconSetPreview();
    WdgPreferencesPage::discardPreviewPreferences();
}

QString WdgPreferencesPageAppearance::id() {
    return "base.appearance";
}

QString WdgPreferencesPageAppearance::name() {
    return tr("Appearance");
}

QIcon WdgPreferencesPageAppearance::icon() {
    return SM::IconServiceImpl::instance()->icon("sun-moon");
}

void WdgPreferencesPageAppearance::setStyleIndex(const int &index) {
    ui->cbStyle->setCurrentIndex(index);
}

void WdgPreferencesPageAppearance::onIconSetChanged(const int &index) {
    QString id = _iconSetsModel->iconSet(ui->cbIconSet->currentIndex());
    SM::IconServiceImpl::instance()->previewIconSet(id);
}

void WdgPreferencesPageAppearance::onStyleChanged(const int &index) {
    Q_UNUSED(index);
    QString id = _stylesModel->style(ui->cbStyle->currentIndex());
    SM::AppearanceServiceImpl::instance()->previewStyle(id);
    SMA::StyleConfig style = SM::AppearanceServiceImpl::instance()->style(id);
    ui->flGeneral->setRowVisible(3, style.lightSupport && style.darkSupport);
    ui->flGeneral->setRowVisible(4, style.accentColorSupport);
}

void WdgPreferencesPageAppearance::onAccentColorChanged(const QString &id) {
    SM::AppearanceServiceImpl::instance()->previewAccentColor(id);
}

void WdgPreferencesPageAppearance::onColorSchemeChanged(const Qt::ColorScheme &colorScheme) {
    SM::AppearanceServiceImpl::instance()->previewColorScheme(colorScheme);
}

void WdgPreferencesPageAppearance::onFontChanged(const QFont &f) {
    SM::AppearanceServiceImpl::instance()->previewUiFontFamily(f.family());
}
