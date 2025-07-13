#include "ProjectManagerImpl.h"

#include "Global/LastUsedFilesManager.h"

#include "MainWindow.h"

#include <QFileDialog>


ProjectManagerImpl::ProjectManagerImpl(QObject *parent) : IProjectManagerInterface(parent), _project(new ProjectData(this)), _fileHandler(new ProjectFileHandler(_project, this)) {}

IProjectDataInterface *ProjectManagerImpl::project() const{
    return _project;
}

ProjectData *ProjectManagerImpl::projectImpl() const {
    return _project;
}

ProjectFileHandler *ProjectManagerImpl::fileHandler() const {
    return _fileHandler;
}

QStringList ProjectManagerImpl::lastUsedProjectFiles() const {
    return LastUsedFilesManager::lastUsedFiles();
}

bool ProjectManagerImpl::newProject() {
    if(!closeProject())
        return false;

    qDebug() << "Create new project...";

    _project->reset();
    _mainWindow->requestNewProject();
    return true;
}

bool ProjectManagerImpl::openProject() {
    const QString url = _mainWindow->requestOpenProjectDialog();
    if(url.isEmpty())
        return false;

    return openProjectFromLocation(url);
}

bool ProjectManagerImpl::openProjectFromLocation(const QString &url) {
    if(!closeProject())
        return false;

    qDebug() << "Open project file" << url;
    _project->reset();
    _fileHandler->readFile(url);
    _mainWindow->requestOpenProject();
    LastUsedFilesManager::addLastUsedFile(url);
    return true;
}

bool ProjectManagerImpl::saveProject() {
    qInfo() << "Save project";
    if(_project->isKnownFile()) {
        return saveProjectToLocation(_project->filePath());
    } else
        return saveProjectAs();
}

bool ProjectManagerImpl::saveProjectAs() {
    const QString url = _mainWindow->requestSaveProjectAsDialog();

    if(url.isEmpty())
        return false;

    const bool ok = saveProjectToLocation(url);
    if(ok)
        LastUsedFilesManager::addLastUsedFile(url);

    return ok;
}

bool ProjectManagerImpl::saveProjectToLocation(const QString &url) {
    if(!_mainWindow->requestSaveProject())
        return false;

    return true;
    _fileHandler->saveFile(url, false); // TODO
}

bool ProjectManagerImpl::closeProject() {
    if(!_mainWindow->requestProjectClose())
        return false;

    qInfo().noquote() << "Closing project" << _project->filePath();
    _project->reset();
    return true;
}
void ProjectManagerImpl::setMainWindow(MainWindow *mainWindow) {
    _mainWindow = mainWindow;
}
