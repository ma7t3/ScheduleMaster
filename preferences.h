#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QtCore>

class Preferences : QObject
{
    Q_OBJECT
public:
    Preferences(QObject *parent);

    static const int LanguageEnglish = 0;
    static const int LanguageGerman = 1;

    int language() const;
    void setLanguage(int newLanguage);

    static Preferences& instance() {
        static Preferences _instance(nullptr);
        return _instance;
    }

private:
    QSettings settingsGeneral = QSettings("ScheduleMaster", "general");
    int _language;
};
#endif // PREFERENCES_H
