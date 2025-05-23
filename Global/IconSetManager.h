#ifndef ICONSETMANAGER_H
#define ICONSETMANAGER_H

#include "Global/GlobalConfigManager.h"

#include <QObject>

struct IconSetConfig : public GlobalConfigItem {
public:
    IconSetConfig(const QJsonObject &jsonObject = QJsonObject(), const int &index = 0);
    IconSetConfig(const QString &id, const int &index = 0);

    QString name, alternative, format;

protected:
    static QString resolvePathPlaceholders(QString path);
};


class IconSetManager : public GlobalConfigManager<IconSetManager, IconSetConfig> {
    friend class GlobalConfigManager<IconSetManager, IconSetConfig>;
    Q_OBJECT
protected:
    explicit IconSetManager(QObject *);

public:
    static QString currentIconSet();

    static void setCurrentIconSet(const QString &iconSetID);

signals:
    void currentIconSetChanged(const QString &iconSetID);
};

#endif // ICONSETMANAGER_H
