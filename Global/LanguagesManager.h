#ifndef LANGUAGESMANAGER_H
#define LANGUAGESMANAGER_H

#include <QObject>

#include "Global/GlobalConfigManager.h"

struct Language : public GlobalConfigItem {
    Language(const QString &id);
    Language(const QJsonObject &json = QJsonObject());

    QLocale locale;
};

class LanguagesManager : public GlobalConfigManager<LanguagesManager, Language> {
    friend class GlobalConfigManager<LanguagesManager, Language>;
    Q_OBJECT
protected:
    explicit LanguagesManager(QObject *parent);

public:
    static QList<Language> supportedLanguages();
    static bool isSupported(const QString &languageID);

    LanguagesManager(const LanguagesManager &) = delete;
    LanguagesManager &operator=(const LanguagesManager &) = delete;
};

#endif // LANGUAGESMANAGER_H
