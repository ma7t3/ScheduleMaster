#ifndef ILANGUAGESERVICE_H
#define ILANGUAGESERVICE_H

#include "ScheduleMaster_global.h"
#include "GlobalConfigItem.h"

namespace ScheduleMaster {

struct LanguageConfig : public GlobalConfigItem {
    LanguageConfig(const QString &id, const int &index = 0);
    LanguageConfig(const QJsonObject &json = QJsonObject(), const int &index = 0);

    QLocale locale;
};

class SCHEDULEMASTERINTERFACE_EXPORT ILanguageService {
public:
    virtual ~ILanguageService() = default;

    virtual QList<LanguageConfig> supportedLanguages() const = 0;
    virtual QStringList supportedLanguageNames() const = 0;
    virtual bool isSupported(const QString &languageID) const = 0;

    virtual QLocale currentLocale() const = 0;
    virtual QLocale::Language currentLanguage() const = 0;
    virtual void setCurrentLanguage(const QString &languageID) = 0;

    virtual bool registerLanguage(LanguageConfig language) = 0;
};

}

#endif // ILANGUAGESERVICE_H
