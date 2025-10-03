#include "IconController.h"

#include "Global/IconSetManager.h"

#include <QApplication>
#include <QStyleHints>
#include <QIcon>

IconController::IconController(QObject *parent) : QObject(parent) {}

IconController *IconController::instance() {
    static IconController instance(nullptr);
    return &instance;
}

void IconController::init() {
    _currentIconSetID = IconSetManager::currentIconSet();
    connect(IconSetManager::instance(), &IconSetManager::currentIconSetChanged, instance(), &IconController::onCurrentIconSetChanged);
    connect(QApplication::styleHints(), &QStyleHints::colorSchemeChanged, instance(), []() {
        emit instance()->currentIconSetChanged(_currentIconSetID);
    });
}

void IconController::applyIconSet(const QString &iconSetID) {
    _currentIconSetID = iconSetID;
    qDebug() << "iconSet set to " << iconSetID;
    emit instance()->currentIconSetChanged(iconSetID);
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
    const bool dark = QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    QString filePath = ":/Icons/" + config.id() + "/" + (dark ? "dark" : "light") + "/" + iconID + "." + config.format;
    if(QFile::exists(filePath))
        return filePath;

    filePath = ":/Icons/" + config.id() + "/" + (dark ? "light" : "dark") + "/" + iconID + "." + config.format;
    if(QFile::exists(filePath))
        return filePath;

    return ":/Icons/" + config.id() + "/" + iconID + "." + config.format;
}
