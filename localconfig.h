#ifndef LOCALCONFIG_H
#define LOCALCONFIG_H

#include <QtCore>

class LocalConfig : QObject
{
    Q_OBJECT
public:
    LocalConfig(QObject *parent);

    static const int LanguageEnglish = 0;
    static const int LanguageGerman = 1;

    static int language();
    static void setLanguage(const int &newLanguage);

    static QStringList lastUsedFiles();
    static void addLastUsedFile(const QString &);
    static void setLastUsedFiles(const QStringList &);
    static void resetLastUsedFiles();

    static bool compressFiles();
    static void setCompressFiles(const bool &newCompressFiles);

private:
    static QSettings settingsGeneral;
};
#endif // LOCALCONFIG_H
