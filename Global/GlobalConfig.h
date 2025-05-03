#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <QObject>
#include <QCoreApplication>
#include <QMetaType>
#include <QColor>
#include <QLocale>
#include <QStandardPaths>
#include <QKeySequence>

#include <QDir>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QIcon>

#include "Global/VariantConverter.h"
#include "Global/Global.h"



// TMP INCLUDE
#include "GlobalConfigManager.h"

class Style : public GlobalConfigItem {
public:
    Style(const QJsonObject &jsonObject = QJsonObject());
    Style(const QString &id);

    bool supportsColorScheme(const Qt::ColorScheme &) const;
    QList<Qt::ColorScheme> supportedColorSchemes() const;

    enum StyleType {
        SystemDefaultType,
        StyleClassType,
        StyleSheetType,
        StyleFactoryType,
        InvalidType
    };

    QString name, styleClassID, styleSheetUrl, styleFactoryName;
    StyleType type;
    bool lightSupport, darkSupport, accentColorSupport, applyPalette;
};


/**
 * @brief The GlobalConfig class provides information about the global configuration of the app.
 *
 * In contrast to the LocalConfig class, which contains user- oder installation-specific configuration, the GlobalConfig is defined by the app itself using the \ref configuration-resource-system "configuration resource system".
 * It contains information about the supported languages, themes, or existing folder locations.
 * The GlobalConfig can be extended or changed during runtime by the app itself or using plugins.
 * For example, if a Plugin adds a new language, the plugin itself can register it on startup using the GlobalConfig class.
 *
 * **Note:** The GlobalConfig is a completely static class that is defined using the singleton-prinziple.
 * You can retrive an instance using the instance() function, but this is only recommended if you need to work with signals or slots which cannot be static.
 * All other functions are static and should be called directly using the class name.
 */
class GlobalConfig : public QObject {
    Q_OBJECT
private:
    /// Private constructor for the signleton instance
    explicit GlobalConfig();

public:
    GlobalConfig(const GlobalConfig &) = delete;
    GlobalConfig &operator=(const GlobalConfig &) = delete;

    /// Return the singleton instance of the GlobalConfig class.
    static GlobalConfig *instance();

    /// Initializes all the rest excluding the supported languages
    static void init();

    /**
     * @brief Returns a map of all registered styles. The map key is the style ID.
     * @return The styles map
     */
    static QMap<QString, Style> styles();

    /**
     * @brief Checks if a style specified by the given ID exists.
     * @param id The style ID
     * @return Whether the style exists or not
     */
    static bool styleExists(const QString &id);

    /**
     * @brief Returns the style specified by the given ID.
     * @param id The style ID
     * @return The Style object or a default constructed style if it doesn't exist
     */
    static Style style(const QString &id);


    /**
     * @brief Returns the type of the style specified by the given ID.
     * @param id The style ID
     * @return The type of the style or Style::StyleType::InvalidType if the style doesn't exist.
     */
    static Style::StyleType styleType(const QString &id);

    /**
     * @brief Returns the accent color specified by the given ID
     * @param id The accent color's ID
     * @return The accent color
     */
    static QColor accentColor(const QString &id);

    /**
     * @brief Returns a darker version of the accent color specified by the given ID
     * @param id The accent color's ID
     * @return The daker version of the accent color
     */
    static QColor accentColorDark(const QString &id);

    /**
     * @brief Returns a list of all accent colors
     * @return The list of accent colors
     */
    static QMap<QString, QColor> accentColors();


    /**
     * @brief Loads a single config resource file and returns the parsed JSON document.
     *
     * This always automatically resolves translated strings.
     * @param resource The resource name (without the .json extension)
     * @return The parsed JSON document or a null document if the parsing fails
     */
    static QJsonDocument loadSingleConfigResource(const QString &resource);

    /**
     * @brief Loads a multi config resource file and returns a JSON array containing all parsed items.
     *
     * This always automatically resolves translated strings.
     * @param resource The resource/directory name
     * @return A JSON array containing all parsed items.
     */
    static QJsonArray loadMultiConfigResource(const QString &resource);


protected:

    /**
     * @brief Reads a JSON file and returns the parsed JSON document.
     *
     * It automatically resolves translated strings.
     * @param fileName The file name
     * @return The parsed JSON document or a null document if the parsing fails
     */
    static QJsonDocument parseJsonFile(const QString &fileName);

    /**
     * @brief Resolves all translated strings inside the JSON object.
     *
     * The return value may still be an object or a string if the given object was a string-translate object which was resolved to a string.
     *
     * This method is executed recursive.
     * @param jsonObject The JSON object to resolve
     * @return The resolved JSON value.
     */
    static QJsonValue resolveTranslatedStrings(QJsonObject jsonObject);

    /**
     * @brief Resolves all translated strings inside the JSON array.
     *
     * This method is executed recursive.
     * @param jsonArray The JSON array to resolve
     * @return The resolved JSON array
     */
    static QJsonArray resolveTranslatedStrings(QJsonArray jsonArray);

    /// Loads all styles
    static void loadStyles();

    /// Loads the accent color palette
    static void loadAccentColors();


private:
    /// All available styles
    static inline QMap<QString, Style> _styles;

    /// All accent colors
    static inline QMap<QString, QColor> _accentColors;
};

#endif // GLOBALCONFIG_H
