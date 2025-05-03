#include "StyleHandler.h"

StyleHandler::StyleHandler() :
    QObject(nullptr) {}

StyleHandler::~StyleHandler() {}

StyleHandler *StyleHandler::instance() {
    static StyleHandler instance;
    return &instance;
}

void StyleHandler::init() {
    QStringList keys = QStyleFactory::keys();

    // when making ScheduleMaster available for other operating systems, this needs to be more platform-specific
    _systemStyleName = keys.contains("windowsvista") ? "windowsvista" : !keys.isEmpty() ? keys.first() : "";

    connect(LocalConfig::instance(), &LocalConfig::accentColorChanged, StyleHandler::instance(), &StyleHandler::applyAccentColor);
    connect(LocalConfig::instance(), &LocalConfig::styleChanged,       StyleHandler::instance(), &StyleHandler::applyStyle);
    connect(LocalConfig::instance(), &LocalConfig::colorSchemeChanged, StyleHandler::instance(), &StyleHandler::applyColorScheme);

    connect(QApplication::styleHints(), &QStyleHints::colorSchemeChanged, instance(), &StyleHandler::onColorSchemeChange);

    _initPalette = QApplication::palette();
}

void StyleHandler::registerStyleClass(const QString &id, QStyle *style) {
    _styleClasses.insert(id, style);
}

void StyleHandler::applyFont(const QString &fontFamily) {
    QFont font = QApplication::font();
    font.setFamily(fontFamily);
    QApplication::setFont(font);
}

void StyleHandler::onColorSchemeChange() {
    applyAccentColor(_currentAccentColor);
}

void StyleHandler::applyPalette() {
    if(_currentStyle.applyPalette)
        QApplication::setPalette(QApplication::style()->standardPalette());
    else
        QApplication::setPalette(_initPalette);
}

void StyleHandler::applyStyle(const QString &id) {
    if(_currentStyle.id() == id)
        return;

    if(!StyleManager::itemExists(id)) {
        qWarning().noquote() << "Cannot apply style " << id << " because it wasn't found.";
        return;
    }

    qDebug().noquote() << "apply style: " << id;

    Style style = StyleManager::item(id);

    switch(style.type) {
    case Style::StyleClassType:
        // TODO
        break;
    case Style::StyleSheetType:
        // TODO
        break;
    case Style::StyleFactoryType:
        if(QStyleFactory::keys().contains(style.styleFactoryName))
            QApplication::setStyle(style.styleFactoryName);
        break;

    case Style::SystemDefaultType:
        QApplication::setStyle(_systemStyleName);
        break;
    case Style::InvalidType: break;
    }

    _currentStyle = style;

    // re-apply palette
    applyPalette();

    // re-apply accent color
    applyAccentColor(_currentAccentColor);
}

void StyleHandler::applyColorScheme(const Qt::ColorScheme &colorScheme) {
    if(QApplication::styleHints()->colorScheme() == colorScheme)
        return;

    const QString colorSchemeName = (colorScheme == Qt::ColorScheme::Unknown ? "auto" : colorScheme == Qt::ColorScheme::Light ? "light" : "dark");

    if(!_currentStyle.supportsColorScheme(colorScheme)) {
        qWarning().noquote() << "cannot apply color scheme " + colorSchemeName + ". Style " + _currentStyle.id() + " doesn't support it.";
        return;
    }

    qDebug().noquote() << "apply color scheme: " << (colorScheme == Qt::ColorScheme::Unknown ? "auto" : colorScheme == Qt::ColorScheme::Light ? "light" : "dark");
    QApplication::styleHints()->setColorScheme(colorScheme);
}

void StyleHandler::applyAccentColor(const QString &id) {
    qDebug().noquote() << "apply accent color: " << id;

    applyPalette();
    QColor color = StyleManager::accentColor(id);
    if(color.isValid()) {
        QPalette palette = QApplication::palette();
        palette.setColor(QPalette::Highlight, color);
        QApplication::setPalette(palette);
    }

    _currentAccentColor = id;
}
