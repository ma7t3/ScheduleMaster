#ifndef PROJECTFILEHANDLER_H
#define PROJECTFILEHANDLER_H

#include <QObject>
#include <QThread>
#include <QApplication>
#include <QFile>
#include <QMutex>

#include "ProjectData/ProjectData.h"

class ProjectFileHandler : public QThread {
    Q_OBJECT
public:
    explicit ProjectFileHandler(ProjectData *projectData, QObject *parent = nullptr);
    ~ProjectFileHandler();

    enum FinishReason {
        SuccessfulReason,
        ErrorReason,
        CancelReason
    };

    void readFile(const QString &filePath);
    void saveFile(const QString &filePath, const bool &compress);

protected:
    virtual void run() override;
    void beforeStart(const QString &filePath);
    bool startStep(const QString &text);
    void finishRun(const FinishReason &reason);

    enum WorkMode {
        ReadMode,
        WriteMode,
        UnknownMode
    };

protected slots:
    void afterFinish();

signals:
    void progressStepChanged(const QString &text);
    void progressStepMaximum(const int &maximum);
    void progressStepUpdate(const int &value);

private:
    WorkMode _workMode;
    bool _compress;
    QString _filePath;
    ProjectData *_projectData;

    static const inline QByteArray uncompressedHeader = "SMP0";
    static const inline QByteArray compressedHeader   = "SMP1";
};

#endif // PROJECTFILEHANDLER_H
