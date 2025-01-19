#ifndef LOCALCONFIG_H
#define LOCALCONFIG_H

#include <QObject>
#include <QSettings>
#include <QLocale>

class LocalConfig : public QObject {
    Q_OBJECT
private:
    explicit LocalConfig();

public:
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

    static void init();

    static LocalConfig *instance();

    static QList<QLocale::Language> supportedLanguages();
    static void addSupportedLanguage(const QLocale::Language &);

    static QLocale locale();
    static QLocale::Language language();
    static void setLanguage(const QLocale::Language &);
    static void setLanguage(const QString &);

    static Style style();
    static void setStyle(const Style &newStyle);

    static QStringList lastUsedFiles();
    static void addLastUsedFile(const QString &);
    static void setLastUsedFiles(const QStringList &);
    static void removeLastUsedFile(const QString &);
    static void resetLastUsedFiles();

    static bool crashDetected();
    static void setCrashDetected(const bool &newCrashDetected);

    static QString lastLogfileName();
    static void setLastLogfileName(const QString &);

    static QString defaultProjectLocation();
    static void setDefaultProjectLocation(const QString &);

    static LogfileMode logfileMode();
    static void setLogfileMode(const LogfileMode &);

    static Qt::ToolBarArea workspacesToolbarPosition();
    static void setWorkspacesToolbarPosition(const Qt::ToolBarArea &);

    static QByteArray mainWindowGeometry();
    static void setMainWindowGeomentry(const QByteArray &geometry);

private:
    static inline QSettings settingsGeneral = QSettings("ScheduleMaster", "general");
    static inline QString _lastLogfileName = "";

    static inline QLocale _locale;

    static inline QList<QLocale::Language> _supportedLanguages = {
        QLocale::English,
        QLocale::German
    };

};

#endif // LOCALCONFIG_H
