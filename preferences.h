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

    static int language();
    static void setLanguage(const int &newLanguage);

private:
    static QSettings settingsGeneral;
};
#endif // PREFERENCES_H
