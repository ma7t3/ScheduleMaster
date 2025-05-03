#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QObject>

#include "Global/GlobalConfigManager.h"

struct LanguageConfig : public GlobalConfigItem {
    LanguageConfig(const QString &id);
    LanguageConfig(const QJsonObject &json = QJsonObject());

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

    LanguageManager(const LanguageManager &) = delete;
    LanguageManager &operator=(const LanguageManager &) = delete;
};

#endif // LANGUAGEMANAGER_H
