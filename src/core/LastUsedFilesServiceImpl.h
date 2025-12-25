#ifndef LASTUSEDFILESSERVICEIMPL_H
#define LASTUSEDFILESSERVICEIMPL_H

#include <QObject>

#include "Service.h"
#include "src/api/ILastUsedFilesService.h"

namespace ScheduleMaster::Core {

class LastUsedFilesServiceImpl :
    public QObject,
    public Service<LastUsedFilesServiceImpl>,
    public ILastUsedFilesService {
    Q_OBJECT

public:
    explicit LastUsedFilesServiceImpl(QObject *parent = nullptr);

    virtual QStringList lastUsedFiles() const override;
    virtual void addLastUsedFile(const QString &filePath) override;
    virtual void removeLastUsedFile(const QString &filePath) override;
    virtual void setLastUsedFiles(const QStringList &list) override;
    virtual void clearLastUsedFiles() override;

signals:
    void lastUsedFilesChanged(const QStringList &lastUsedFiles);
};
}

#endif // LASTUSEDFILESSERVICEIMPL_H
