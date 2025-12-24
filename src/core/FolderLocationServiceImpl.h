#ifndef FOLDERLOCATIONSERVICEIMPL_H
#define FOLDERLOCATIONSERVICEIMPL_H

#include "GlobalConfigRepository.h"
#include "GlobalConfigService.h"

#include "src/api/IFolderLocationService.h"

namespace ScheduleMaster::Core {

using FolderLocationRepository = GlobalConfigRepositoryCRTP<FolderLocationConfig>;

class FolderLocationServiceImpl :
    public GlobalConfigServiceCRTP<FolderLocationRepository, FolderLocationServiceImpl>,
    public IFolderLocationService {
    Q_OBJECT

public:
    explicit FolderLocationServiceImpl(QObject *parent);

    virtual QMap<QString, QStringList> currentFolderLocations() const override;
    virtual QStringList currentFolderLocationPaths(const QString &folderLocationID) const override;
    virtual void setCurrentFolderLocationPaths(const QString &folderLocationID, const QStringList &paths) override;
    virtual void setCurrentFolderLocations(const QMap<QString, QStringList> &folderLocations) override;
    virtual bool folderLocationExists(const QString &id) const override;
    virtual FolderLocationConfig folderLocation(const QString &id) const override;
    virtual QList<FolderLocationConfig> folderLocations() const override;
    virtual bool registerFolderLocations(FolderLocationConfig folderLocation) override;

signals:
    void currentFolderLocationPathsChanged(const QString &folderLocationID, const QStringList &paths);
};
}

#endif // FOLDERLOCATIONSERVICEIMPL_H
