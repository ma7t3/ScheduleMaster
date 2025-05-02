#ifndef FOLDERLOCATIONSHANDLER_H
#define FOLDERLOCATIONSHANDLER_H

#include <QObject>
#include <QStandardPaths>
#include <QApplication>

#include "Global/GlobalConfigHandler.h"
#include "Global/GlobalConfig.h"

class FolderLocation : public GlobalConfigItem {
public:
    FolderLocation(const QJsonObject &jsonObject = QJsonObject());
    FolderLocation(const QString &id);

    QString name, icon;
    QStringList defaultPaths;
    bool multiple, requiresRestart;

protected:
    static QString resolvePathPlaceholders(QString path);
};


class FolderLocationsHandler : public GlobalConfigHandler<FolderLocation> {
    Q_OBJECT
private:
    explicit FolderLocationsHandler(QObject *parent);

public:
    static FolderLocationsHandler *instance();

    static void init();

    FolderLocationsHandler(const FolderLocationsHandler &) = delete;
    FolderLocationsHandler &operator=(const FolderLocationsHandler &) = delete;
};

#endif // FOLDERLOCATIONSHANDLER_H
