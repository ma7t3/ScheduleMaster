#ifndef LOCALCONFIG_H
#define LOCALCONFIG_H

#include <QtCore>

class LocalConfig : QObject {
    Q_OBJECT
public:
    LocalConfig(QObject *parent);

    enum Language {
        English,
        German
    };

    enum LogfileMode {
        NoLog,
        DefaultLog,
        DebugLog,
        DebugDetailLog
    };

    enum Style {
        System,
        WindowsXpStyle,
        Fusion
    };

    enum TimeFormat {
        Hours12,
        Hours24
    };

    static Language language();
    static void setLanguage(const Language &newLanguage);

    static TimeFormat timeFormat();
    static void setTimeFormat(const TimeFormat &newTimeFormat);
    static QString timeFormatString(const bool &showSeconds, const bool &oneDigitHour);

    static Style style();
    static void setStyle(const Style &newStyle);

    static QStringList lastUsedFiles();
    static void addLastUsedFile(const QString &);
    static void setLastUsedFiles(const QStringList &);
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

private:
    static inline QSettings settingsGeneral = QSettings("ScheduleMaster", "general");
    static inline QString _lastLogfileName = "";
    static inline bool _currentTimeFormatInitialized = false;
    static inline TimeFormat _currentTimeFormat;
};
#endif // LOCALCONFIG_H
