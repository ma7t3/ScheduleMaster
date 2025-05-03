#include "LastUsedFilesManager.h"

#include "SettingsManager.h"

LastUsedFilesManager::LastUsedFilesManager(QObject *parent) : QObject(parent) {}

LastUsedFilesManager *LastUsedFilesManager::instance() {
    static LastUsedFilesManager instance(nullptr);
    return &instance;
}

void LastUsedFilesManager::init() {
    instance();
}

QStringList LastUsedFilesManager::lastUsedFiles() {
    return SettingsManager::value("general.lastUsedFiles").toStringList();
}

void LastUsedFilesManager::addLastUsedFile(const QString &filePath) {
    QStringList list = lastUsedFiles();
    list.removeAll(filePath);
    list.insert(0, filePath);
    setLastUsedFiles(list);
}

void LastUsedFilesManager::setLastUsedFiles(const QStringList &list) {
    QStringList modifiedList = list;
    modifiedList.replaceInStrings("\\", "/");
    SettingsManager::setValue("general.lastUsedFiles", modifiedList);
    emit instance()->lastUsedFilesChanged();
}

void LastUsedFilesManager::removeLastUsedFile(const QString &filePath) {
    QStringList list = lastUsedFiles();
    list.removeAll(filePath);
    setLastUsedFiles(list);
}

void LastUsedFilesManager::clearLastUsedFiles() {
    SettingsManager::setValue("general.lastUsedFiles", QStringList{});
}
