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

    static Language language();
    static void setLanguage(const Language &newLanguage);

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

private:
    static inline QSettings settingsGeneral = QSettings("ScheduleMaster", "general");
    static inline QString _lastLogfileName = "";
};
#endif // LOCALCONFIG_H
