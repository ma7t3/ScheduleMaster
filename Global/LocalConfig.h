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
#include <QApplication>

#include "Global/GlobalConfig.h"
#include "Global/LanguagesManager.h"
#include "Global/FolderLocationsManager.h"
#include "Global/SettingsManager.h"
#include "Global/KeyboardShortcutsManager.h"


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

    static LocalConfig *instance();

    static void initLocale();
    static void init();

    static QVariant read(const QString &id);
    static QVariant write(const QString &id, const QVariant &value);
    static void remove(const QString &id);
    static QVariant restoreDefault(const QString &id);
    static void rename(const QString &oldID, const QString &newID);
    static bool keyExsists(const QString &key);
    static QStringList groupKeys(const QString &group);
    static QStringList groupSubGroups(const QString &group);
    static QStringList allKeys();
    static QStringList allGroups();

    static bool restartRequired();
    static QStringList restartRequiredSettings();

    static QLocale locale();
    static QLocale::Language language();
    static void setLanguage(const QLocale &);
    static void setLanguage(const QString &);

    static QString style();
    static void setStyle(const QString &newStyle);

    static Qt::ColorScheme colorScheme();
    static void setColorScheme(const Qt::ColorScheme &);

    static QString uiFontFamily();
    static void setUiFontFamily(const QString &fontFamily);

    static QString accentColorID();
    static QColor accentColor();
    static void setAccentColor(const QString &id);

    static bool useGdiEngine();
    static void setUseGdiEngine(const bool &useGdiEngine);

    static QMap<QString, QStringList> folderLocations();
    static void setFolderLocations(const QMap<QString, QStringList> &locations);

    static QStringList folderLocationPaths(const QString &id);
    static void setFolderLocationPaths(const QString &id, const QStringList &paths);

    static QKeySequence keyboardShortcut(const QString &id);
    static void setKeyboardShortcut(const QString &id, const QKeySequence &keySequence);

    static QStringList lastUsedFiles();
    static void addLastUsedFile(const QString &);
    static void setLastUsedFiles(const QStringList &);
    static void removeLastUsedFile(const QString &);
    static void resetLastUsedFiles();

    static bool crashDetected();
    static void setCrashDetected(const bool &newCrashDetected);

    static QString lastLogfileName();
    static void setLastLogfileName(const QString &);

    static bool currentLogfileExists();

    static LogfileMode logfileMode();
    static void setLogfileMode(const LogfileMode &);

    static Qt::ToolBarArea workspacesToolbarPosition();
    static void setWorkspacesToolbarPosition(const Qt::ToolBarArea &);

    static QByteArray mainWindowGeometry();
    static void setMainWindowGeomentry(const QByteArray &geometry);

    static void importKeyboardShortcutsFromJson(const QJsonArray &jsonArray);
    static QJsonArray exportKeyboardShortcutsToJson();

protected:
    static QVariant readSilent(const QString &id);

signals:
    void settingAdded(const QString &id, const QVariant &value);
    void settingChanged(const QString &id, const QVariant &value);
    void settingRemoved(const QString &id);

    void lastUsedFilesChanged();
    void uiFontChanged(QString);
    void accentColorChanged(QString);
    void styleChanged(QString);
    void colorSchemeChanged(Qt::ColorScheme);

    void keyboardShortcutChanged(const QString &id, const QKeySequence &keySequence);

private:
    static inline QSettings settings = QSettings("ScheduleMaster", "ScheduleMaster");

    static inline QString _lastLogfileName = "";

    static inline QLocale _locale;

    static inline QSet<QString> _modifiedRestartRequiredSettings;

    static inline bool _logfileCreated = false;
};

#endif // LOCALCONFIG_H
