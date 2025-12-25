#ifndef LANGUAGESERVICEIMPL_H
#define LANGUAGESERVICEIMPL_H

#include "GlobalConfigRepository.h"
#include "GlobalConfigService.h"

#include "src/api/ILanguageService.h"

namespace ScheduleMaster::Core {

using LanguageRepository = GlobalConfigRepositoryCRTP<LanguageConfig>;

class LanguageServiceImpl : public GlobalConfigServiceCRTP<LanguageRepository, LanguageServiceImpl>,
public ILanguageService {
    Q_OBJECT

public:
    explicit LanguageServiceImpl(QObject *parent);

    virtual QList<LanguageConfig> supportedLanguages() const override;
    virtual QStringList supportedLanguageNames() const override;
    virtual bool isSupported(const QString &languageID) const override;

    virtual QLocale currentLocale() const override;
    virtual QLocale::Language currentLanguage() const override;
    virtual void setCurrentLanguage(const QString &languageID) override;

    virtual bool registerLanguage(LanguageConfig language) override;

signals:
    void currentLanguageChanged(const QString &languageID);
};

} // namespace ScheduleMaster::Core

#endif // LANGUAGESERVICEIMPL_H
