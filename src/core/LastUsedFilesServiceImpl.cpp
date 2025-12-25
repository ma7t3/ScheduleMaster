#include "LastUsedFilesServiceImpl.h"

#include "src/core/SettingsServiceImpl.h"

namespace ScheduleMaster::Core {

LastUsedFilesServiceImpl::LastUsedFilesServiceImpl(QObject *parent) : QObject(parent) {}

QStringList LastUsedFilesServiceImpl::lastUsedFiles() const {
    return SettingsServiceImpl::instance()->value("general.lastUsedFiles").toStringList();
}

void LastUsedFilesServiceImpl::addLastUsedFile(const QString &filePath) {
    QStringList list = lastUsedFiles();
    list.removeAll(filePath);
    list.insert(0, filePath);
    setLastUsedFiles(list);
}

void LastUsedFilesServiceImpl::removeLastUsedFile(const QString &filePath) {
    QStringList list = lastUsedFiles();
    list.removeAll(filePath);
    setLastUsedFiles(list);
}

void LastUsedFilesServiceImpl::setLastUsedFiles(const QStringList &list) {
    QStringList modifiedList = list;
    modifiedList.replaceInStrings("\\", "/");
    SettingsServiceImpl::instance()->setValue("general.lastUsedFiles", modifiedList);
    emit lastUsedFilesChanged(modifiedList);
}

void LastUsedFilesServiceImpl::clearLastUsedFiles() {
    setLastUsedFiles(QStringList{});
}

}
