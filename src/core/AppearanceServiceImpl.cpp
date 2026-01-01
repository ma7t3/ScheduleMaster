#include "AppearanceServiceImpl.h"

#include <QApplication>
#include <QFont>
#include <QStyleFactory>
#include <QStyleHints>

#include "src/core/SettingsServiceImpl.h"

namespace ScheduleMaster::Core {

AppearanceServiceImpl::AppearanceServiceImpl(QObject *parent) : GlobalConfigServiceCRTP(parent, "Styles"), _previewColorScheme(-1) {
    initRepository();

    _initPalette = QApplication::palette();

    connect(SettingsServiceImpl::instance(), &SettingsServiceImpl::valueChanged, this, [this](const QString &key, const QVariant &value) {
        qDebug() << "Appearance changed" << key << value;

        if(key == "appearance.accentColor") {
            endAccentColorPreview();
            applyAccentColor(value.toString());
        }

        if(key == "appearance.theme") {
            endStylePreview();
            applyStyle(value.toString());
        }

        if(key == "appearance.colorScheme") {
            endColorSchemePreview();
            applyColorScheme(colorSchemeFromVariant(value));
        }

        if(key == "appearance.font") {
            endUiFontFamilyPreview();
            applyFont(value.toString());
        }
    });

    connect(QApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, [this]() {
        applyAccentColor(_currentAccentColor);
    });
}

void AppearanceServiceImpl::applyAppearance() {
    applyStyle(currentStyleID());
    applyColorScheme(currentColorScheme());
    applyAccentColor(currentAccentColorID());
    applyFont(currentUiFontFamily());
}

QColor AppearanceServiceImpl::accentColor(const QString &accentColorID) const {
    return repository()->accentColor(accentColorID);
}

QMap<QString, QColor> AppearanceServiceImpl::accentColors() const {
    return repository()->accentColors();
}

QString AppearanceServiceImpl::currentAccentColorID() const {
    return isAccentColorPreviewEnabled() ? _previewAccentColorID : SettingsServiceImpl::instance()->value("appearance.accentColor").toString();
}

QColor AppearanceServiceImpl::currentAccentColor() const {
    return accentColor(currentAccentColorID());
}

void AppearanceServiceImpl::setCurrentAccentColor(const QString &accentColorID) {
    SettingsServiceImpl::instance()->setValue("appearance.accentColor", accentColorID);
    endAccentColorPreview();
}

void AppearanceServiceImpl::previewAccentColor(const QString &accentColorID) {
    _previewAccentColorID = accentColorID;
    applyAccentColor(accentColorID);
}

void AppearanceServiceImpl::endAccentColorPreview() {
    _previewAccentColorID.clear();
    applyAccentColor(currentAccentColorID());
}

bool AppearanceServiceImpl::isAccentColorPreviewEnabled() const {
    return !_previewAccentColorID.isEmpty();
}

QList<StyleConfig> AppearanceServiceImpl::styles() const {
    return repository()->items();
}

QStringList AppearanceServiceImpl::styleIDs() const {
    return repository()->itemIDs();
}

StyleConfig AppearanceServiceImpl::style(const QString &id) const {
    return repository()->item(id);
}

QString AppearanceServiceImpl::currentStyleID() const {
    return isStylePreviewEnabled() ? _previewStyleID : SettingsServiceImpl::instance()->value("appearance.theme").toString();
}

QStyle *AppearanceServiceImpl::currentStyle() const {
    // TODO
    return nullptr;
}

void AppearanceServiceImpl::setCurrentStyle(const QString &styleID) {
    if(!repository()->itemExists(styleID))
        return;

    SettingsServiceImpl::instance()->setValue("appearance.theme", styleID);
    endStylePreview();
}

bool AppearanceServiceImpl::registerStyle(StyleConfig styleConfig) {
    return repository()->addItem(styleConfig);
}

void AppearanceServiceImpl::previewStyle(const QString &styleID) {
    _previewStyleID = styleID;
    applyStyle(styleID);
}

void AppearanceServiceImpl::endStylePreview() {
    _previewStyleID.clear();
    applyStyle(currentStyleID());
}

bool AppearanceServiceImpl::isStylePreviewEnabled() const {
    return !_previewStyleID.isEmpty();
}

Qt::ColorScheme AppearanceServiceImpl::currentColorScheme() const {
    return isColorSchemePreviewEnabled()
               ? static_cast<Qt::ColorScheme>(_previewColorScheme)
               : colorSchemeFromVariant(
                     SettingsServiceImpl::instance()->value("appearance.colorScheme"));
}

void AppearanceServiceImpl::setCurrentColorScheme(const Qt::ColorScheme &colorScheme) {
    SettingsServiceImpl::instance()->setValue("appearance.colorScheme", static_cast<int>(colorScheme));
    endColorSchemePreview();
}

void AppearanceServiceImpl::previewColorScheme(const Qt::ColorScheme &colorScheme) {
    _previewColorScheme = static_cast<int>(colorScheme);
    applyColorScheme(colorScheme);
}

void AppearanceServiceImpl::endColorSchemePreview() {
    _previewColorScheme = -1;
    applyColorScheme(currentColorScheme());
}

bool AppearanceServiceImpl::isColorSchemePreviewEnabled() const {
    return _previewColorScheme != -1;
}

QString AppearanceServiceImpl::currentUiFontFamily() const {
    const QString str = SettingsServiceImpl::instance()->value("appearance.font").toString();
    if(str.isEmpty())
        return QApplication::font().family();
    else
        return str;
}

void AppearanceServiceImpl::setCurrentUiFontFamily(const QString &fontFamily) {
    SettingsServiceImpl::instance()->setValue("appearance.font", fontFamily);
    endUiFontFamilyPreview();
}

void AppearanceServiceImpl::previewUiFontFamily(const QString &fontFamily) {
    _previewUiFontFamily = fontFamily;
    applyFont(fontFamily);
}

void AppearanceServiceImpl::endUiFontFamilyPreview() {
    _previewUiFontFamily.clear();
    applyFont(currentUiFontFamily());
}

bool AppearanceServiceImpl::isUiFontFamilyPreviewEnabled() const {
    return !_previewUiFontFamily.isEmpty();
}

Qt::ColorScheme AppearanceServiceImpl::colorSchemeFromVariant(const QVariant &value) {
    bool ok;
    const int intVal = value.toInt(&ok);
    if(!ok)
        return Qt::ColorScheme::Unknown;
    return colorSchemeFromInt(intVal);
}

Qt::ColorScheme AppearanceServiceImpl::colorSchemeFromInt(const int &value) {
    if(value < 0 || value > 2)
        return Qt::ColorScheme::Unknown;

    return static_cast<Qt::ColorScheme>(value);
}

void AppearanceServiceImpl::applyStyle(const QString &id) {
    if(_currentStyle.id() == id)
        return;

    if(!repository()->itemExists(id)) {
        qWarning().noquote() << "Cannot apply style " << id << " because it wasn't found.";
        return;
    }

    qDebug().noquote() << "apply style: " << id;

    const StyleConfig style = repository()->item(id);

    switch(style.type) {
    case StyleConfig::StyleClassType:
        // TODO
        break;
    case StyleConfig::StyleSheetType:
        // TODO
        break;
    case StyleConfig::StyleFactoryType:
        if(QStyleFactory::keys().contains(style.styleFactoryName)) {
            QApplication::setStyle(style.styleFactoryName);
        } else {
            qWarning().noquote() << "Cannot apply style " << id << " because it is currently not supported.";
        }
        break;
        break;
    case StyleConfig::InvalidType: break;
    }

    _currentStyle = style;

    // re-apply palette
    applyPalette();

    // re-apply accent color
    applyAccentColor(_currentAccentColor);

    emit currentStyleChanged(id);
}

void AppearanceServiceImpl::applyColorScheme(const Qt::ColorScheme &colorScheme) {
    if(QApplication::styleHints()->colorScheme() == colorScheme)
        return;

    const QString colorSchemeName = (colorScheme == Qt::ColorScheme::Unknown ? "auto"
                                     : colorScheme == Qt::ColorScheme::Light ? "light"
                                                                             : "dark");

    if(!_currentStyle.supportsColorScheme(colorScheme)) {
        qWarning().noquote() << "cannot apply color scheme " + colorSchemeName + ". Style " + _currentStyle.id() + " doesn't support it.";
        return;
    }

    qDebug().noquote() << "apply color scheme: "
                       << (colorScheme == Qt::ColorScheme::Unknown ? "auto"
                           : colorScheme == Qt::ColorScheme::Light ? "light"
                                                                   : "dark");
    QApplication::styleHints()->setColorScheme(colorScheme);
    emit currentColorSchemeChanged(colorScheme);
}

void AppearanceServiceImpl::applyAccentColor(const QString &id) {
    if(!_currentStyle.accentColorSupport)
        return;

    qDebug().noquote() << "apply accent color: " << id;

    applyPalette();
    const QColor color = accentColor(id);
    if(color.isValid()) {
        QPalette palette = QApplication::palette();
        palette.setColor(QPalette::Highlight, color);
        QApplication::setPalette(palette);
    }

    _currentAccentColor = id;

    emit currentAccentColorChanged(id);
}

void AppearanceServiceImpl::applyFont(const QString &fontFamily) {
    QFont font = QApplication::font();
    font.setFamily(fontFamily);
    QApplication::setFont(font);
    emit currentUiFontFamilyChanged(fontFamily);
}

void AppearanceServiceImpl::applyPalette() {
    if(_currentStyle.applyPalette)
        QApplication::setPalette(QApplication::style()->standardPalette());
    else
        QApplication::setPalette(_initPalette);
}

} // namespace ScheduleMaster::Core
