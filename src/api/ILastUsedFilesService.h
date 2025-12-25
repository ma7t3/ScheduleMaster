#ifndef ILASTUSEDFILESSERVICE_H
#define ILASTUSEDFILESSERVICE_H

#include "ScheduleMaster_global.h"

namespace ScheduleMaster {

class SCHEDULEMASTERINTERFACE_EXPORT ILastUsedFilesService {
public:
    virtual ~ILastUsedFilesService() = default;

    virtual QStringList lastUsedFiles() const = 0;
    virtual void addLastUsedFile(const QString &filePath) = 0;
    virtual void setLastUsedFiles(const QStringList &list) = 0;
    virtual void removeLastUsedFile(const QString &filePath) = 0;
    virtual void clearLastUsedFiles() = 0;
};

}

#endif // ILASTUSEDFILESSERVICE_H
