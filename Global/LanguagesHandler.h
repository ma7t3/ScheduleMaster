#ifndef LANGUAGESHANDLER_H
#define LANGUAGESHANDLER_H

#include <QObject>

#include "Global/GlobalConfigHandler.h"

struct Language : public GlobalConfigItem {
    Language(const QString &id);
    Language(const QJsonObject &json = QJsonObject());

    QLocale locale;
};

class LanguagesHandler : public GlobalConfigHandler<Language> {
    Q_OBJECT
private:
    explicit LanguagesHandler(QObject *parent);

public:
    static LanguagesHandler *instance();

    static void init();

    static QList<Language> supportedLanguages();
    static bool isSupported(const QString &languageID);

    LanguagesHandler(const LanguagesHandler &) = delete;
    LanguagesHandler &operator=(const LanguagesHandler &) = delete;
};

#endif // LANGUAGESHANDLER_H
