#ifndef IICONSERVICE_H
#define IICONSERVICE_H

#include "ScheduleMaster_global.h"
#include "GlobalConfigItem.h"

namespace ScheduleMaster {

struct IconSetConfig : public GlobalConfigItem {
public:
    IconSetConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    IconSetConfig(const QString &id, const int &index = 0);

    QString name, alternative, format;

protected:
    static QString resolvePathPlaceholders(QString path);
};

class SCHEDULEMASTERINTERFACE_EXPORT IIconService {
public:
    virtual ~IIconService() = default;

    virtual QList<IconSetConfig> iconSets() const = 0;
    virtual bool registerIconSet(const IconSetConfig &iconSetConfig) = 0;

    virtual QString currentIconSet() const = 0;
    virtual void setCurrentIconSet(const QString &iconSetID) = 0;

    virtual QIcon icon(const QString &iconID) const = 0;
};

}

#endif // IICONSERVICE_H
