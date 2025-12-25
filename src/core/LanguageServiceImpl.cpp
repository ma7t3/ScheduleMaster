#include "LanguageServiceImpl.h"

#include "src/core/SettingsServiceImpl.h"

namespace ScheduleMaster::Core {

LanguageServiceImpl::LanguageServiceImpl(QObject *parent) : GlobalConfigServiceCRTP(parent, "Languages") {
    initRepository();

    QString str = SettingsServiceImpl::instance()->value("general.language").toString();
    QLocale tmpLocale(str);
    QLocale locale;
    if(str == "system" || !isSupported(tmpLocale.name()))
        locale = QLocale::system();
    else
        locale = QLocale(str);

    QLocale::setDefault(locale);
}

QList<LanguageConfig> LanguageServiceImpl::supportedLanguages() const {
    return repository()->items();
}

QStringList LanguageServiceImpl::supportedLanguageNames() const {
    QStringList list;
    for(const LanguageConfig &conf : supportedLanguages())
        list << conf.locale.name();
    return list;
}

bool LanguageServiceImpl::isSupported(const QString &languageID) const {
    return repository()->itemExists(languageID);
}

QLocale LanguageServiceImpl::currentLocale() const {
    return QLocale(SettingsServiceImpl::instance()->value("general.language").toString());
}

QLocale::Language LanguageServiceImpl::currentLanguage() const {
    return currentLocale().language();
}

void LanguageServiceImpl::setCurrentLanguage(const QString &languageID) {
    if(!isSupported(languageID)) {
        qWarning() << "Cannot set current language to" << languageID + "." << "Not supported!";
        return;
    }

    SettingsServiceImpl::instance()->setValue("general.language", languageID);
    emit currentLanguageChanged(languageID);
}

bool LanguageServiceImpl::registerLanguage(LanguageConfig language) {
    return repository()->addItem(language);
}
}
