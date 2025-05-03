#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QObject>
#include <QColor>
#include <QMap>

#include "GlobalConfigManager.h"

class StyleConfig : public GlobalConfigItem {
public:
    /**
     * @brief Constructor
     * @param jsonObject The JSON object containing the style configuration
     */
    StyleConfig(const QJsonObject &jsonObject = QJsonObject());

    /**
     * @brief Constructor
     * @param id The ID of the style
     */
    StyleConfig(const QString &id);

    /**
     * @brief Returns whether the style supports the given color scheme
     * @param colorScheme The color scheme to check
     * @return true if the style supports the given color scheme, false otherwise
     */
    bool supportsColorScheme(const Qt::ColorScheme &colorScheme) const;

    /**
     * @brief Returns a list of all color schemes supported by the style
     * @return The list of color schemes
     */
    QList<Qt::ColorScheme> supportedColorSchemes() const;

    /**
     * @brief The StyleType enum describes the type of a StyleConfig object
     */
    enum StyleType {
        SystemDefaultType,  ///< The style describes the system's default style
        StyleClassType,     ///< The style is defined using a QStyle subclass
        StyleSheetType,     ///< The style is represented by a QSS stylesheet
        StyleFactoryType,   ///< The style represents a QStyleFactory key
        InvalidType         ///< The style is invalid
    };

    QString name, styleClassID, styleSheetUrl, styleFactoryName;
    StyleType type;
    bool lightSupport, darkSupport, accentColorSupport, applyPalette;
};

class StyleManager : public GlobalConfigManager<StyleManager, StyleConfig> {
    friend class GlobalConfigManager<StyleManager, StyleConfig>;
    Q_OBJECT
public:
    explicit StyleManager(QObject *parent);

    /**
     * @brief Returns the accent color specified by the given ID
     * @param id The accent color's ID
     * @return The accent color
     */
    static QColor accentColor(const QString &id);

    /**
     * @brief Returns a darker version of the accent color specified by the given ID
     * @param id The accent color's ID
     * @return The darker version of the accent color
     */
    static QColor accentColorDark(const QString &id);

    /**
     * @brief Returns a list of all accent colors
     * @return The list of accent colors
     */
    static QMap<QString, QColor> accentColors();

    /**
     * @brief Returns the current style
     * @return The current style's ID
     */
    static QString currentStyle();

    /**
     * @brief Sets the current style
     * @param styleID The ID of the style to set
     */
    static void setCurrentStyle(const QString &styleID);

    /**
     * @brief Returns the current color scheme
     * @return The current color scheme
     */
    static Qt::ColorScheme currentColorScheme();

    /**
     * @brief Sets the current color scheme
     * @param colorScheme The color scheme to set
     */
    static void setCurrentColorScheme(const Qt::ColorScheme &colorScheme);

    /**
     * @brief Returns the current accent color's ID
     * @return The current accent color's ID
     */
    static QString currentAccentColorID();

    /**
     * @brief Returns the current accent color
     * @return The current accent color
     */
    static QColor currentAccentColor();

    /**
     * @brief Sets the current accent color
     * @param accentColorID The ID of the accent color to set
     */
    static void setCurrentAccentColor(const QString &accentColorID);

    /**
     * @brief Returns the current UI font family
     * @return The current UI font family
     */
    static QString currentUiFontFamily();

    /**
     * @brief Sets the current UI font family
     * @param fontFamily The font family to set
     */
    static void setCurrentUiFontFamily(const QString &fontFamily);

    /**
     * @brief Returns whether the GDI font engine is enabled
     * @return true if the GDI font engine is enabled, false otherwise
     */
    static bool gdiFontEngineEnabled();

    /**
     * @brief Sets whether the GDI font engine is enabled
     * @param enabled true to enable the GDI font engine, false to disable it
     */
    static void setGdiFontEngineEnabled(const bool &enabled);

protected:
    /// Loads the accent colors
    static void loadAccentColors();

signals:
    /**
     * @brief Emitted when the current style changes
     * @param styleID The new style's ID
     */
    void styleChanged(const QString &styleID);

    /**
     * @brief Emitted when the current color scheme changes
     * @param colorScheme The new color scheme
     */
    void colorSchemeChanged(const Qt::ColorScheme &colorScheme);

    /**
     * @brief Emitted when the current accent color changes
     * @param accentColorID The new accent color's ID
     */
    void accentColorChanged(const QString &accentColorID);

    /**
     * @brief Emitted when the current UI font family changes
     * @param fontFamily The new UI font family
     */
    void uiFontFamilyChanged(const QString &fontFamily);

private:
    /// All accent colors
    static inline QMap<QString, QColor> _accentColors;
};

#endif // STYLEMANAGER_H

