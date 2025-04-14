#ifndef LOCALCONFIG_H
#define LOCALCONFIG_H

#include <QGuiApplication>
#include <QMainWindow>
#include <QFont>

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

    static void initLocale();
    static void init();

    static QVariant read(const QString &id);
    static void write(const QString &id, const QVariant &value = QVariant());
    static bool keyExsists(const QString &key);
    static QStringList groupKeys(const QString &group);
    static QStringList groupSubGroups(const QString &group);
    static QStringList allKeys();
    static QStringList allGroups();

    static bool restartRequired();
    static QStringList restartRequiredSettings();

public:
    static QLocale locale();
    static QLocale::Language language();
    static void setLanguage(const QLocale &);
    static void setLanguage(const QString &);

    static Style style();
    static void setStyle(const Style &newStyle);

    static QString uiFontFamily();
    static void setUiFontFamily(const QString &fontFamily);
    static void previewUiFontFamily(const QString &fontFamily);
    static void previewUiFontFamily();
    static void restoreUiFontFamilyPreview();

    static bool useGdiEngine();
    static void setUseGdiEngine(const bool &useGdiEngine);

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
    static QStringList folderLocationDefaultValue(const QString &id);

signals:
    void lastUsedFilesChanged();
    void uiFontChanged(const QString &fontFamily);

private:
    static inline QSettings settings = QSettings("ScheduleMaster", "ScheduleMaster");

    static inline QString _lastLogfileName = "";

    static inline QLocale _locale;

};

#endif // LOCALCONFIG_H
