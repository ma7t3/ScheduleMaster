#ifndef ICONSERVICEIMPL_H
#define ICONSERVICEIMPL_H

#include "GlobalConfigRepository.h"
#include "GlobalConfigService.h"

#include "api/IIconService.h"

namespace ScheduleMaster::Core {

using IconSetRepository = GlobalConfigRepositoryCRTP<IconSetConfig>;
using IconXdgMappingRepository = GlobalConfigRepositoryCRTP<IconXdgMappingConfig>;

class IconServiceImpl :
    public GlobalConfigServiceCRTP<IconSetRepository, IconServiceImpl>,
    public IIconService {
    Q_OBJECT

public:
    explicit IconServiceImpl(QObject *parent = nullptr);

    virtual QList<IconSetConfig> iconSets() const override;
    virtual bool registerIconSet(const IconSetConfig &iconSetConfig) override;

    virtual QString currentIconSet() const override;
    virtual void setCurrentIconSet(const QString &iconSetID) override;

    virtual bool preferSystemIcons() const override;
    virtual void setPreferSystemIcons(bool preferSystemIcons) override;

    virtual bool registerXdgMapping(const IconXdgMappingConfig &xdgMappingConfig) override;

    void previewIconSet(const QString &iconSetID);
    void discardIconSetPreview();
    bool isIconSetPreviewEnabled() const;

    virtual QIcon icon(const QString &iconID) const override;

protected:
    QString xdgMappedIconName(const QString &iconID) const;
    static QString createFilePath(const QString &iconID, const IconSetConfig &config);

signals:
    void currentIconSetChanged(const QString &iconSetID);

private:
    IconXdgMappingRepository *_xdgMappingRepository;
    QString _currentIconSetID, _currentIconSetPreviewID;

    mutable QHash<QString, QString> _xdgMappingCache;
};

} // namespace ScheduleMaster::Core

#endif // ICONSERVICEIMPL_H
