#include "IconController.h"

#include "Global/IconSetManager.h"

#include <QIcon>

IconController::IconController(QObject *parent) : QObject(parent) {}

IconController *IconController::instance() {
    static IconController instance(nullptr);
    return &instance;
}

void IconController::init() {
    _currentIconSetID = IconSetManager::currentIconSet();
    connect(IconSetManager::instance(), &IconSetManager::currentIconSetChanged, instance(), &IconController::onCurrentIconSetChanged);
}

void IconController::applyIconSet(const QString &iconSetID) {
    _currentIconSetID = iconSetID;
    qDebug() << "iconSet set to " << iconSetID;
    // FIXME
}

QIcon IconController::icon(const QString &iconID) {
    QStringList triedSets;
    QString currentIconSetID = _currentIconSetID;
    while(true) {
        IconSetConfig current = IconSetManager::item(currentIconSetID);
        QString filePath = createFilePath(iconID, current);
        if(QFile::exists(filePath))
            return QIcon(filePath);

        triedSets << currentIconSetID;
        currentIconSetID = current.alternative;
        if(triedSets.contains(currentIconSetID))
            return QIcon();
    }
}

void IconController::onCurrentIconSetChanged(const QString &iconSetID) {
    applyIconSet(iconSetID);
}

QString IconController::createFilePath(const QString &iconID, const IconSetConfig &config) {
    return ":/Icons/" + config.id() + "/" + iconID + "." + config.format;
}
