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

    void setAction(const Action &);
    void setFilePath(const QString &);

private:
    void readFile();
    void saveFile();

signals:
    void actionStarted(const DlgProgressLogger::EntryType &, const QString &, const bool &showAsCurrentAction = false);

    void unknownVersionDetected();
    void oldVersionDetected(AppInfo::AppVersion *);

private:
    ProjectData *projectData;

    Action _action;
    QString _filePath;

    const QByteArray fileHeaderCompressed = QString("SMP1").toUtf8();
    const QByteArray fileHeaderUncompressed = QString("SMP0").toUtf8();

};

#endif // FILEHANDLER_H
