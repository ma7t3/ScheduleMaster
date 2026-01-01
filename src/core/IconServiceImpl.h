#ifndef ICONSERVICEIMPL_H
#define ICONSERVICEIMPL_H

#include "GlobalConfigRepository.h"
#include "GlobalConfigService.h"

#include "src/api/IIconService.h"

namespace ScheduleMaster::Core {

using IconSetRepository = GlobalConfigRepositoryCRTP<IconSetConfig>;

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

    void previewIconSet(const QString &iconSetID);
    void discardIconSetPreview();
    bool isIconSetPreviewEnabled() const;

    virtual QIcon icon(const QString &iconID) const override;

protected:
    static QString createFilePath(const QString &iconID, const IconSetConfig &config);

signals:
    void currentIconSetChanged(const QString &iconSetID);

private:
    QString _currentIconSetID, _currentIconSetPreviewID;

};

} // namespace ScheduleMaster::Core

#endif // ICONSERVICEIMPL_H
