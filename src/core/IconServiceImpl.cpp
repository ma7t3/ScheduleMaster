#include "IconServiceImpl.h"

#include <QApplication>
#include <QStyleHints>
#include <QIcon>

#include "core/SettingsServiceImpl.h"

namespace ScheduleMaster::Core {

IconServiceImpl::IconServiceImpl(QObject *parent) : GlobalConfigServiceCRTP(parent, "IconSets"),
    _xdgMappingRepository{new IconXdgMappingRepository(this, "IconXdgMappings")} {

    connect(_xdgMappingRepository, &IconXdgMappingRepository::stateChanged, this, [this]() {
        _xdgMappingCache.clear();
    });

    _xdgMappingRepository->init();

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

bool IconServiceImpl::preferSystemIcons() const {
    return SettingsServiceImpl::instance()->value("appearance.preferSystemIcons").toBool();
}

void IconServiceImpl::setPreferSystemIcons(bool preferSystemIcons) {
    SettingsServiceImpl::instance()->setValue("appearance.preferSystemIcons", preferSystemIcons);
    emit currentIconSetChanged(currentIconSet());
}

bool IconServiceImpl::registerXdgMapping(const IconXdgMappingConfig &xdgMappingConfig) {
    return _xdgMappingRepository->addItem(xdgMappingConfig);
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
    if(SettingsServiceImpl::instance()->value("appearance.preferSystemIcons").toBool()) {
        QString xdgIconName = xdgMappedIconName(iconID);
        if(!xdgIconName.isEmpty() && QIcon::hasThemeIcon(xdgIconName))
            return QIcon::fromTheme(xdgIconName);
    }

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

QString IconServiceImpl::xdgMappedIconName(const QString &iconID) const {
    if(_xdgMappingCache.contains(iconID))
        return _xdgMappingCache.value(iconID);

    QString foundName;
    const auto mappingConfigs = _xdgMappingRepository->sortedItems();
    for(const auto &mappingConfig : mappingConfigs) {
        const QString name = mappingConfig.mappings.value(iconID, "");
        if(!name.isEmpty())
            foundName = name;
    }

    _xdgMappingCache.insert(iconID, foundName);
    return foundName;
}

QString IconServiceImpl::createFilePath(const QString &iconID, const IconSetConfig &config) {
    const bool dark = QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    QString filePath = ":/icons/" + config.id() + "/" + (dark ? "dark" : "light") + "/" + iconID + "." + config.format;
    if(QFile::exists(filePath))
        return filePath;

    filePath = ":/icons/" + config.id() + "/" + (dark ? "light" : "dark") + "/" + iconID + "." + config.format;
    if(QFile::exists(filePath))
        return filePath;

    return ":/icons/" + config.id() + "/" + iconID + "." + config.format;
}

}
