#ifndef LASTUSEDFILESMANAGER_H
#define LASTUSEDFILESMANAGER_H

#include <QObject>

class LastUsedFilesManager : public QObject {
    Q_OBJECT
private:
    explicit LastUsedFilesManager(QObject *parent);

public:
    static LastUsedFilesManager *instance();
    static void init();

    static QStringList lastUsedFiles();
    static void addLastUsedFile(const QString &filePath);
    static void setLastUsedFiles(const QStringList &list);
    static void removeLastUsedFile(const QString &filePath);
    static void clearLastUsedFiles();

    LastUsedFilesManager(const LastUsedFilesManager &other) = delete;
    LastUsedFilesManager &operator=(const LastUsedFilesManager &other) = delete;

signals:
    void lastUsedFilesChanged();
};

#endif // LASTUSEDFILESMANAGER_H
