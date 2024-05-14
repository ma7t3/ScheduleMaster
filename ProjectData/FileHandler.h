#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "ProjectData/projectdata.h"
#include "Dialogs/DlgProgressLogger.h"
#include "AppInfo.h"

#include <QObject>
#include <QThread>

class FileHandler : public QThread {
    Q_OBJECT
public:
    explicit FileHandler(QObject *parent = nullptr);

    enum Action {
        ReadFileAction,
        SaveFileAction
    };

    void run() override;

    Action action() const;
    void setAction(const Action &);

    QString filePath() const;
    void setFilePath(const QString &);

    bool success() const;

private:
    bool readFile();
    bool saveFile();

signals:
    void actionStarted(const DlgProgressLogger::EntryType &, const QString &, const bool &showAsCurrentAction = false);

    void unknownVersionDetected();
    void oldVersionDetected(AppInfo::AppVersion *);

    void openFileError(const QString &filePath, const QString errorString);
    void saveFileError(const QString &filePath, const QString errorString);

private:
    ProjectData *projectData;

    Action _action;
    QString _filePath;
    bool _succes;

    const QByteArray fileHeaderCompressed = QString("SMP1").toUtf8();
    const QByteArray fileHeaderUncompressed = QString("SMP0").toUtf8();

};

#endif // FILEHANDLER_H
