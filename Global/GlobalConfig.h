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

class GlobalConfig : public QObject {
    Q_OBJECT
private:
    explicit GlobalConfig();

public:
    GlobalConfig(const GlobalConfig &) = delete;
    GlobalConfig &operator=(const GlobalConfig &) = delete;

    static GlobalConfig *instance();

    static void initLanguages();
    static void init();

    static QList<QLocale> supportedLanguages();
    static bool languageIsSupported(const QLocale &language);
    static bool languageIsSupported(const QString &languageName);
    static void addSupportedLanguage(const QLocale &newLanguage);

    static QList<FolderLocation> folderLocations();
    static void setFolderLocationName(const QString &id, const QString &name);

protected:
    static QJsonArray loadConfigResource(const QString &resource);
    static QJsonValue resolveConfigResourceTranslatedStrings(QJsonObject jsonObject);
    static QJsonArray resolveConfigResourceTranslatedStrings(QJsonArray jsonArray);
    static void loadSupportedLanguages();
    static void loadNativeFolderLocations();

signals:

private:
    static inline QSet<QLocale> _supportedLanguages;
    static inline QMap<QString, FolderLocation> _folderLocations;
};

#endif // GLOBALCONFIG_H
