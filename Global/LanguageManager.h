#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include "Global/GlobalConfigManager.h"

#include <QObject>

struct LanguageConfig : public GlobalConfigItem {
    LanguageConfig(const QString &id, const int &index = 0);
    LanguageConfig(const QJsonObject &json = QJsonObject(), const int &index = 0);

    QLocale locale;
};

class LanguageManager : public GlobalConfigManager<LanguageManager, LanguageConfig> {
    friend class GlobalConfigManager<LanguageManager, LanguageConfig>;
    Q_OBJECT
protected:
    explicit LanguageManager(QObject *parent);

public:
    static QList<LanguageConfig> supportedLanguages();
    static bool isSupported(const QString &languageID);

    static QLocale currentLocale();
    static QLocale::Language currentLanguage();
    static void setCurrentLanguage(const QString &languageID);

    LanguageManager(const LanguageManager &) = delete;
    LanguageManager &operator=(const LanguageManager &) = delete;

signals:
    void currentLanguageChanged(const QString &languageID);
};

#endif // LANGUAGEMANAGER_H
