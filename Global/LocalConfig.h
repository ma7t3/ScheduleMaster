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

#include "Global/GlobalConfig.h"

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

    static QLocale locale();
    static QLocale::Language language();
    static void setLanguage(const QLocale &);
    static void setLanguage(const QString &);

    static Style style();
    static void setStyle(const Style &newStyle);

    static QMap<QString, QStringList> folderLocations();
    static void setFolderLocations(const QMap<QString, QStringList> &locations);

    static QStringList folderLocationPaths(const QString &id);
    static void setFolderLocationPaths(const QString &id, const QStringList &paths);


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
    void static loadFolderLocations();

signals:
    void lastUsedFilesChanged();

private:
    static inline QSettings settingsGeneral   = QSettings("ScheduleMaster", "general");
    static inline QSettings settingsLocations = QSettings("ScheduleMaster", "locations");
    static inline QString _lastLogfileName = "";

    static inline QLocale _locale;

    static inline QMap<QString, QStringList> _folderLocations;

};

#endif // LOCALCONFIG_H
