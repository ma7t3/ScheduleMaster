#ifndef PROJECTMANAGERIMPL_H
#define PROJECTMANAGERIMPL_H

#include <ScheduleMasterInterface/IScheduleMasterInterface.h>

#include "ProjectData/ProjectData.h"
#include "Global/ProjectFileHandler.h"

class MainWindow;

class ProjectManagerImpl : public IProjectManagerInterface {
    Q_OBJECT
public:
    ProjectManagerImpl(QObject *parent);

    IProjectDataInterface *project() const override;
    ProjectData *projectImpl() const;

    ProjectFileHandler *fileHandler() const;
    QStringList lastUsedProjectFiles() const override;

    void setMainWindow(MainWindow *mainWindow);

public slots:
    bool newProject() override;
    bool openProject() override;
    bool openProjectFromLocation(const QString &url) override;
    bool saveProject() override;
    bool saveProjectAs() override;
    bool saveProjectToLocation(const QString &url) override;
    bool closeProject() override;

protected:
    ProjectData *_project;
    ProjectFileHandler *_fileHandler;

    MainWindow *_mainWindow;
};

#endif // PROJECTMANAGERIMPL_H
