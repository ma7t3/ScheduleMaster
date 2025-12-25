#include "IconServiceImpl.h"

#include <QApplication>
#include <QStyleHints>
#include <QIcon>

#include "src/core/SettingsServiceImpl.h"

namespace ScheduleMaster::Core {

IconServiceImpl::IconServiceImpl(QObject *parent) : GlobalConfigServiceCRTP(parent, "IconSets") {
    initRepository();
    _currentIconSetID = SettingsServiceImpl::instance()->value("appearance.iconSet").toString();
    connect(SettingsServiceImpl::instance(),
            &SettingsServiceImpl::valueChanged,
            this,
            [this](const QString &id, const QVariant &value) {
                if(id != "appearance.iconSet")
                    return;

                _currentIconSetID = value.toString();
                emit currentIconSetChanged(currentIconSet());
            });

    connect(QApplication::styleHints(), &QStyleHints::colorSchemeChanged, instance(), [this]() {
        emit currentIconSetChanged(currentIconSet());
    });
}

QList<IconSetConfig> IconServiceImpl::iconSets() const {
    return repository()->items();
}

bool IconServiceImpl::registerIconSet(const IconSetConfig &iconSetConfig) {
    return repository()->addItem(iconSetConfig);
}

QString IconServiceImpl::currentIconSet() const {
    return isIconSetPreviewEnabled() ? _currentIconSetPreviewID : _currentIconSetID;
}

void IconServiceImpl::setCurrentIconSet(const QString &iconSetID) {
    SettingsServiceImpl::instance()->setValue("appearance.iconSet", iconSetID);
    discardIconSetPreview();
}

void IconServiceImpl::previewIconSet(const QString &iconSetID) {
    _currentIconSetPreviewID = iconSetID;
    emit currentIconSetChanged(currentIconSet());
}

void IconServiceImpl::discardIconSetPreview() {
    _currentIconSetPreviewID.clear();
    emit currentIconSetChanged(currentIconSet());
}

bool IconServiceImpl::isIconSetPreviewEnabled() const {
    return !_currentIconSetPreviewID.isEmpty();
}

QIcon IconServiceImpl::icon(const QString &iconID) const {
    QStringList triedSets;
    QString currentIconSetID = currentIconSet();

    while(true) {
        IconSetConfig current = repository()->item(currentIconSetID);

        QString filePath = createFilePath(iconID, current);
        if(QFile::exists(filePath))
            return QIcon(filePath);

        triedSets << currentIconSetID;
        currentIconSetID = current.alternative;
        if(triedSets.contains(currentIconSetID))
            return QIcon();
    }
}

QString IconServiceImpl::createFilePath(const QString &iconID, const IconSetConfig &config) {
    const bool dark = QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    QString filePath = ":/Icons/" + config.id() + "/" + (dark ? "dark" : "light") + "/" + iconID + "." + config.format;
    if(QFile::exists(filePath))
        return filePath;

    filePath = ":/Icons/" + config.id() + "/" + (dark ? "light" : "dark") + "/" + iconID + "." + config.format;
    if(QFile::exists(filePath))
        return filePath;

    return ":/Icons/" + config.id() + "/" + iconID + "." + config.format;
}
}
