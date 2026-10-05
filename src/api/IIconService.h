#ifndef IICONSERVICE_H
#define IICONSERVICE_H

#include "ScheduleMaster_global.h"
#include "GlobalConfigItem.h"

namespace ScheduleMaster {

struct IconXdgMappingConfig : public GlobalConfigItem {
public:
    IconXdgMappingConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    IconXdgMappingConfig(const QString &id, const int &index = 0);

    QHash<QString, QString> mappings;
};

struct IconSetConfig : public GlobalConfigItem {
public:
    IconSetConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    IconSetConfig(const QString &id, const int &index = 0);

    QString name, alternative, format;
};

class SCHEDULEMASTERINTERFACE_EXPORT IIconService {
public:
    virtual ~IIconService() = default;

    virtual QList<IconSetConfig> iconSets() const = 0;
    virtual bool registerIconSet(const IconSetConfig &iconSetConfig) = 0;

    virtual QString currentIconSet() const = 0;
    virtual void setCurrentIconSet(const QString &iconSetID) = 0;

    virtual bool preferSystemIcons() const = 0;
    virtual void setPreferSystemIcons(bool preferSystemIcons) = 0;

    virtual bool registerXdgMapping(const IconXdgMappingConfig &xdgMappingConfig) = 0;

    virtual QIcon icon(const QString &iconID) const = 0;
};

}

#endif // IICONSERVICE_H
