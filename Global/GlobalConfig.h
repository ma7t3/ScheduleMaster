#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <QObject>
#include <QLocale>

#include <QDir>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class FolderLocation {
public:
    FolderLocation(){}
    FolderLocation(const QString &id, const QString &name, const QString &icon, const bool &multiple) : id(id), name(name), icon(icon), multiple(multiple) {}
    FolderLocation(const QString &id, const bool &multiple) : id(id), name(""), icon(""), multiple(multiple) {}

    QString id, name, icon;
    bool multiple;
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

    /// Initializes the supported languages.
    static void initLanguages();

    /// Initializes all the rest excluding the supported languages
    static void init();

    /**
     * @brief Returns a list of all suported languages/locales.
     * @return The list of supported languages/locales
     */
    static QList<QLocale> supportedLanguages();

    /**
     * @brief Checks if a given language/locale is supported by the app.
     * @param language The QLocale object to check
     * @return whether the language/locale is being supported or not
     */
    static bool languageIsSupported(const QLocale &language);

    /**
     * @brief This is an overloaded function.
     *
     * Checks if a given language/locale is supported by the app.
     * @param language The language/locale name (e.g. "en_US") to check
     * @return whether the language/locale is being supported or not
     */
    static bool languageIsSupported(const QString &languageName);

    /**
     * @brief Returns a list of all folder locations used by the app.
     * @return The list of all folder locations
     */
    static QList<FolderLocation> folderLocations();

protected:
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

    /// Loads all supported languages
    static void loadSupportedLanguages();

    /// Loads all folder locations
    static void loadFolderLocations();

signals:

private:
    /// Set of all supported languages/locales
    static inline QSet<QLocale> _supportedLanguages;

    /// Map of all folder locations: <id, Folder Location object>
    static inline QMap<QString, FolderLocation> _folderLocations;
};

#endif // GLOBALCONFIG_H
