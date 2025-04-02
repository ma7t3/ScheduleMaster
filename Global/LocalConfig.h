#ifndef LOCALCONFIG_H
#define LOCALCONFIG_H

#include <QObject>
#include <QSettings>
#include <QLocale>
#include <QCoreApplication>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QStandardPaths>

class FolderLocation {
public:
    FolderLocation(){}
    FolderLocation(const QString &id, const QString &name, const QString &icon, const bool &multiple, const QStringList &paths) : id(id), name(name), icon(icon), paths(paths), multiple(multiple) {}
    FolderLocation(const QString &id, const bool &multiple, const QStringList &paths) : id(id), name(""), icon(""), paths(paths), multiple(multiple) {}

    QString id, name, icon;
    QStringList paths;
    bool multiple;
};

class LocalConfig : public QObject {
    Q_OBJECT
private:
    explicit LocalConfig();

public:
    LocalConfig(const LocalConfig &) = delete;
    LocalConfig &operator=(const LocalConfig &) = delete;

    enum LogfileMode {
        NoLog,
        DefaultLog,
        DebugLog,
        DebugDetailLog
    };

    enum Style {
        SystemStyle,
        WindowsXpStyle,
        FusionStyle
    };

    static LocalConfig *instance();

    static void init();

    static QList<QLocale::Language> supportedLanguages();
    static void addSupportedLanguage(const QLocale::Language &);

    static QLocale locale();
    static QLocale::Language language();
    static void setLanguage(const QLocale::Language &);
    static void setLanguage(const QString &);

    static Style style();
    static void setStyle(const Style &newStyle);

    static QList<FolderLocation> folderLocations();
    static QStringList folderLocationPaths(const QString &id);
    static void updateFolderLocation(const FolderLocation &location);
    static void setFolderLocationName(const QString &id, const QString &name);

    static QStringList lastUsedFiles();
    static void addLastUsedFile(const QString &);
    static void setLastUsedFiles(const QStringList &);
    static void removeLastUsedFile(const QString &);
    static void resetLastUsedFiles();

    static bool crashDetected();
    static void setCrashDetected(const bool &newCrashDetected);

    static QString lastLogfileName();
    static void setLastLogfileName(const QString &);

    static LogfileMode logfileMode();
    static void setLogfileMode(const LogfileMode &);

    static Qt::ToolBarArea workspacesToolbarPosition();
    static void setWorkspacesToolbarPosition(const Qt::ToolBarArea &);

    static QByteArray mainWindowGeometry();
    static void setMainWindowGeomentry(const QByteArray &geometry);

protected:
    static QJsonArray loadConfigResource(const QString &resource);
    static void loadSupportedLanguages();
    static void loadNativeFolderLocations();

signals:
    void lastUsedFilesChanged();

private:
    static inline QSettings settingsGeneral   = QSettings("ScheduleMaster", "general");
    static inline QSettings settingsLocations = QSettings("ScheduleMaster", "locations");
    static inline QString _lastLogfileName = "";

    static inline QLocale _locale;

    static inline QList<QLocale::Language> _supportedLanguages;

    static inline QMap<QString, FolderLocation> _folderLocations;

};

#endif // LOCALCONFIG_H
