#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Dialogs/DlgGlobalSearch.h"
#include "Dialogs/DlgPreferences.h"
#include "Global/ActionController.h"
#include "Global/CrashDetector.h"
#include "Global/DockController.h"
#include "Global/FolderLocationManager.h"
#include "Global/LastUsedFilesManager.h"
#include "Global/Logger.h"
#include "Global/ProjectFileHandler.h"
#include "Global/Workspace.h"
#include "Global/WorkspaceHandler.h"
#include "ProjectData/ProjectData.h"
#include "Widgets/Docks/DockAbstract.h"

#include <QTimer>
#include <QDesktopServices>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QToolBar>
#include <QUndoStack>
#include <QUndoView>
#include <QLayout>

#include "InterfaceImpl/AppInterfaceImpl.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _windowOnceShown(false),
    _dockController(nullptr),
    _workspaceHandler(new WorkspaceHandler(this)),
    _projectData(appInterface->projectManagerImpl()->projectImpl()),
    _fileHandler(appInterface->projectManagerImpl()->fileHandler()),
    _globalSearch(new DlgGlobalSearch(this)) {
    ui->setupUi(this);

    qInfo() << "Loading MainWindow";

    ui->centralwidget->hide();

    connectToInterface();

#ifndef QT_DEBUG
    ui->menuDebug->setHidden(true);
#endif

    _workspaceHandler->setWorkspacesMenu(ui->menuWorkspaces);

    qDebug() << "   Loading Undo/redo actions...";
    _undoAction = _projectData->undoStack()->createUndoAction(this, tr("Undo"));
    _redoAction = _projectData->undoStack()->createRedoAction(this, tr("Redo"));

    _openProjectFromFileInRecentFilesMenuAction = addAction(tr("Open Project from File"));
    connect(_openProjectFromFileInRecentFilesMenuAction, &QAction::triggered, ui->actionFileOpenProject, &QAction::trigger);

    qDebug() << "   Loading shortcuts...";

    ActionController::addAsGlobalAction(ui->actionFileNewProject,      "project.new");
    ActionController::addAsGlobalAction(ui->actionFileOpenProject,     "project.open");
    ActionController::add(_openProjectFromFileInRecentFilesMenuAction, "project.open", ActionController::IconComponent);
    ActionController::add(ui->menuFileOpenRecent,                      "project.recentFiles");
    QAction *actionShowRecentFilesList = ActionController::addAsGlobalAction(addAction(""), "project.recentFiles");
    ActionController::addAsGlobalAction(ui->actionFileSaveProject,     "project.save");
    ActionController::addAsGlobalAction(ui->actionFileSaveProjectAs,   "project.saveAs");
    ActionController::addAsGlobalAction(ui->actionFileCloseProject,    "project.close");
    ActionController::addAsGlobalAction(ui->actionFileQuit,            "application.quit");

    ActionController::addAsGlobalAction(ui->actionEditPreferences,     "application.preferences.open");
    ActionController::addAsGlobalAction(ui->actionEditProjectSettings, "project.settings.open");
    ActionController::addAsGlobalAction(ui->actionEditConfiguration,   "application.configuration.open");

    ActionController::addAsGlobalAction(_undoAction,                   "edit.undo");
    ActionController::addAsGlobalAction(_redoAction,                   "edit.redo");

    ActionController::add(ui->menuDocks,                               "view.docks");
    ActionController::add(ui->menuWorkspaces,                          "view.workspaces");
    ActionController::add(ui->actionViewToolbars,                      "view.toolbars");

    QAction *actionShowDockList      = ActionController::addAsGlobalAction(addAction(""), "view.docks");
    QAction *actionShowWorkspaceList = ActionController::addAsGlobalAction(addAction(""), "view.workspaces");

    ActionController::add(ui->actionViewGlobalSearch,    "application.globalSearch.open");


    ui->menuEdit->insertAction(ui->actionEditPreferences, _undoAction);
    ui->menuEdit->insertAction(ui->actionEditPreferences, _redoAction);
    ui->menuEdit->insertSeparator(ui->actionEditPreferences);

    initToolbars();
    loadDocks();

    updateRecentProjectsList();

    connect(ui->actionFileNewProject,         &QAction::triggered,                         appInterface->projectManagerImpl(), &ProjectManagerImpl::newProject);
    connect(ui->actionFileOpenProject,        &QAction::triggered,                         appInterface->projectManagerImpl(), &ProjectManagerImpl::openProject);
    connect(ui->actionFileSaveProject,        &QAction::triggered,                         appInterface->projectManagerImpl(), &ProjectManagerImpl::saveProject);
    connect(ui->actionFileSaveProjectAs,      &QAction::triggered,                         appInterface->projectManagerImpl(), &ProjectManagerImpl::saveProjectAs);
    connect(ui->actionFileCloseProject,       &QAction::triggered,                         appInterface->projectManagerImpl(), &ProjectManagerImpl::closeProject);

    connect(actionShowRecentFilesList,        &QAction::triggered,                         this, &MainWindow::showRecentFilesMenu);
    connect(ui->actionFileQuit,               &QAction::triggered,                         this, &MainWindow::quitApplication);

    connect(ui->actionEditPreferences,        &QAction::triggered,                         this, &MainWindow::openPreferences);
    connect(ui->actionEditConfiguration,      &QAction::triggered,                         this, &MainWindow::openConfiguration);
    connect(ui->actionEditProjectSettings,    &QAction::triggered,                         this, &MainWindow::openProjectSettings);
    connect(ui->actionViewGlobalSearch,       &QAction::triggered,                         this, &MainWindow::showGlobalSearch);

    connect(actionShowDockList,               &QAction::triggered,                         this, &MainWindow::showDockMenu);
    connect(actionShowWorkspaceList,          &QAction::triggered,                         this, &MainWindow::showWorkspaceMenu);

    connect(LastUsedFilesManager::instance(), &LastUsedFilesManager::lastUsedFilesChanged, this, &MainWindow::updateRecentProjectsList);

    connect(_fileHandler,                     &ProjectFileHandler::progressStepChanged,    this, &MainWindow::onFileHandlerProgressStepChanged);
    connect(_fileHandler,                     &ProjectFileHandler::progressStepMaximum,    this, &MainWindow::onFileHandlerProgressMaximum);
    connect(_fileHandler,                     &ProjectFileHandler::progressStepUpdate,     this, &MainWindow::onFileHandlerProgressUpdate);
    connect(_fileHandler,                     &QThread::finished,                          this, &MainWindow::onFileHandlerFinished);

    showCrashWarning();
}

MainWindow::~MainWindow() {
    delete ui;
}

WorkspaceHandler *MainWindow::workspaceHandler() const {
    return _workspaceHandler;
}

bool MainWindow::requestNewProject() {
    _workspaceHandler->switchToOnProjectOpenWorkspace();
    return true;
}

QString MainWindow::requestOpenProjectDialog() {
    return QFileDialog::getOpenFileName(this, tr("Open Project File"), FolderLocationManager::currentFolderLocationPaths("projectFilesDefault").first(), tr("ScheduleMaster Project File (*.smp);;JSON (*.json)"));
}

bool MainWindow::requestOpenProject() {
    createFileHandlerProgressDialog(tr("Open project..."));
    _workspaceHandler->switchToOnProjectOpenWorkspace();
    return true;
}

bool MainWindow::requestSaveProject() {
    return true;
}

QString MainWindow::requestSaveProjectAsDialog() {
    return QFileDialog::getOpenFileName(this, tr("Save Project File"), FolderLocationManager::currentFolderLocationPaths("projectFilesDefault").first(), tr("ScheduleMaster Project File (*.smp);;JSON (*.json)"));
}

bool MainWindow::requestProjectClose() {
    // TODO: Check for unsaved changes
    _workspaceHandler->switchToOnProjectCloseWorkspace();
    return true;
}

void MainWindow::showEvent(QShowEvent *event) {
    if(_windowOnceShown)
        return;

    _windowOnceShown = true;

    // This is shit but there is no way to make it a better way....
    QTimer::singleShot(100, this, [this](){
        Workspace *workspace = _workspaceHandler->onApplicationStartupWorkspace();
        if(workspace)
            workspace->activate();
    });
}

void MainWindow::connectToInterface() {
    // FIXME
    /*ApplicationInterface::init(_projectData);
    connect(ApplicationInterface::instance(), &ApplicationInterface::newProject,                  this, &MainWindow::newProject);
    connect(ApplicationInterface::instance(), &ApplicationInterface::openProject,                 this, &MainWindow::openProject);
    connect(ApplicationInterface::instance(), &ApplicationInterface::openProjectFromFile,         this, &MainWindow::openProjectFromFile);
    connect(ApplicationInterface::instance(), &ApplicationInterface::saveProject,                 this, &MainWindow::saveProject);
    connect(ApplicationInterface::instance(), &ApplicationInterface::saveProjectAs,               this, &MainWindow::saveProjectAs);
    connect(ApplicationInterface::instance(), &ApplicationInterface::closeProject,                this, &MainWindow::closeProjectBackToHome);
    connect(ApplicationInterface::instance(), &ApplicationInterface::quitApplication,             this, &MainWindow::quitApplication);
    connect(ApplicationInterface::instance(), &ApplicationInterface::removeProjectFromRecentList, this, &MainWindow::removeProjectFromRecentList);
    connect(ApplicationInterface::instance(), &ApplicationInterface::openPlugins,                 this, &MainWindow::openPlugins);
    connect(ApplicationInterface::instance(), &ApplicationInterface::openPreferences,             this, &MainWindow::openPreferences);
    connect(ApplicationInterface::instance(), &ApplicationInterface::openConfiguration,           this, &MainWindow::openConfiguration);
    connect(ApplicationInterface::instance(), &ApplicationInterface::openProjectSettings,         this, &MainWindow::openProjectSettings);*/
}

void MainWindow::loadDocks() {
    qInfo() << "   Loading docks...";
    _dockController = new DockController(this);
    connect(_dockController, &DockController::dockAdded, this, &MainWindow::onDockAdded);
    _dockController->loadStandardDocks();
    static_cast<QUndoView *>(_dockController->dock("undoView")->widget()->layout()->itemAt(0)->widget())->setStack(_projectData->undoStack());

    for(QDockWidget *dockWidget : _dockController->dockList()) {
        DockAbstract *dock = qobject_cast<DockAbstract *>(dockWidget->widget());
        if(!dock)
            continue;

        QMenu *menu = dock->globalMenu();
        if(menu->isEmpty())
            continue;

        menu->setTitle(dockWidget->windowTitle());
        ui->menubar->addMenu(menu);
    }
}

void MainWindow::initToolbars() {
    qInfo() << "   Initializing toolbars...";
    _toolbarGeneral    = new QToolBar(tr("General"), this);
    _toolbarDocks      = new QToolBar(tr("Docks"), this);
    _toolbarWorkspaces = new QToolBar(tr("Workspaces"), this);

    _toolbarGeneral   ->setObjectName("toolbarGeneral");
    _toolbarDocks     ->setObjectName("toolbarDocks");
    _toolbarWorkspaces->setObjectName("toolbarWorkspaces");

    _toolbarGeneral->addAction(ui->actionFileNewProject);
    _toolbarGeneral->addAction(ui->actionFileOpenProject);
    _toolbarGeneral->addAction(ui->actionFileSaveProject);
    _toolbarGeneral->addSeparator();
    _toolbarGeneral->addAction(_undoAction);
    _toolbarGeneral->addAction(_redoAction);
    _toolbarGeneral->addSeparator();
    _toolbarGeneral->addAction(ui->actionEditProjectSettings);
    _toolbarGeneral->addAction(ui->actionEditPreferences);

    // docks

    _toolbarWorkspaces->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _workspaceHandler->setWorkspacesToolbar(_toolbarWorkspaces);

    addToolBar(_toolbarGeneral);
    addToolBar(_toolbarDocks);
    addToolBar(_toolbarWorkspaces);
}

void MainWindow::updateRecentProjectsList() {
    qInfo() << "Updating open recent projects list...";

    QMenu *menu = ui->menuFile->findChild<QMenu *>("menuFileOpenRecent");
    if(!menu)
        return;

    menu->clear();

    QStringList list = LastUsedFilesManager::lastUsedFiles();

    int i = 0;
    for(const QString &path : std::as_const(list)) {
        if(i > 9)
            return;

        QString fileName = QFileInfo(path).fileName();

        QAction *action = menu->addAction(QString::number(i) + ": " + fileName);
        action->setToolTip(path);
        connect(action, &QAction::triggered, this, [path](){appInterface->projectManagerImpl()->openProjectFromLocation(path);});
        i++;
    }
    menu->addSeparator();
    menu->addAction(_openProjectFromFileInRecentFilesMenuAction);
}

void MainWindow::showCrashWarning() {
    if(CrashDetector::crashDetected()) {
        const QString lastLogfilePath = Logger::lastLogfilePath();
        bool logfileSaved = QFile::exists(lastLogfilePath);

        qInfo() << "crash detected" + (logfileSaved ? ", logfile saved separately: " + lastLogfilePath : "");
        qInfo().noquote() << "crash detected" + (logfileSaved ? ", logfile saved separately: " + lastLogfilePath : "");
        QString messageTitle = tr("Crash detected");
        QString messageStr = tr("<p><b>Seems like ScheduleMaster crashed last time you used it</b></p><p>If this was unexpected (e.g. you didn't try to kill the process via the Windows Task Manager or something), please feel free to send a bug report!</p>");
        QString messageStrAddition = tr("<p>The logfile of your last session was saved seperately. Do you want to open it?</p>");
        qApp->restoreOverrideCursor();
        if(logfileSaved) {
            messageStr += messageStrAddition;
            QMessageBox::StandardButton msg = QMessageBox::warning(this, messageTitle, messageStr, QMessageBox::Yes|QMessageBox::No);
            if(msg == QMessageBox::Yes) {
                QDesktopServices::openUrl(lastLogfilePath);
            }
        } else {
            QMessageBox::warning(this, messageTitle, messageStr, QMessageBox::Ok);
        }
        qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    }
}

void MainWindow::showRecentFilesMenu() {
    QPoint cursorPos = QCursor::pos();
    ui->menuFileOpenRecent->popup(QPoint(cursorPos.x() - 12, cursorPos.y() - 12));
}

void MainWindow::quitApplication() {
    QApplication::quit();
}

void MainWindow::removeProjectFromRecentList(const QString &filePath) {
    QMessageBox::StandardButton msg = QMessageBox::warning(this,
                         tr("Remove item from recent projects"),
                         tr("<p>Do you really want to remove this item from your recent projects list?</p><p><b>%1</b></p>").arg(filePath),
                         QMessageBox::Yes|QMessageBox::No);

    if(msg != QMessageBox::Yes)
        return;

    LastUsedFilesManager::removeLastUsedFile(filePath);
}

void MainWindow::showDockMenu() {
    QAction *currentAction = nullptr;
    const QList<QAction *> actions = ui->menuDocks->actions();
    for(int i = 0; i < actions.count(); i++) {
        if(!actions[i]->isChecked())
            continue;

        currentAction = actions[i];
        break;
    }

    QPoint cursorPos = QCursor::pos();
    ui->menuDocks->popup(QPoint(cursorPos.x() - 12, cursorPos.y() - 12), currentAction);
}

void MainWindow::showWorkspaceMenu() {
    QAction *currentAction = nullptr;
    const QList<QAction *> actions = ui->menuWorkspaces->actions();
    for(int i = 0; i < actions.count(); i++) {
        if(!actions[i]->isChecked())
            continue;

        currentAction = actions[i];
        break;
    }

    QPoint cursorPos = QCursor::pos();
    ui->menuWorkspaces->popup(QPoint(cursorPos.x() - 12, cursorPos.y() - 12), currentAction);
}

void MainWindow::openPlugins() {
    qInfo() << "Open preferences with plugins page";
    DlgPreferences dlg(this);
    dlg.setCurrentPage("base.plugins");
    dlg.exec();
}

void MainWindow::openPreferences() {
    qInfo() << "Open preferences";
    DlgPreferences dlg(this);
    dlg.exec();
}

void MainWindow::openConfiguration() {
    DlgPreferences dlg(this);
    dlg.openConfigEditor();
}

void MainWindow::openProjectSettings() {
    qInfo() << "Open project settings";
}

void MainWindow::onDockAdded(const QString &id, QDockWidget *dockWidget, QAction *toggleViewAction) {
    ui->menuDocks->addAction(toggleViewAction);
    _toolbarDocks->addAction(toggleViewAction);
}

void MainWindow::createFileHandlerProgressDialog(const QString &title) {
    _fileHandlerProgressDialog = new QProgressDialog("", tr("Cancel"), 0, 0, this);
    _fileHandlerProgressDialog->setWindowTitle(title);
    _fileHandlerProgressDialog->setWindowModality(Qt::ApplicationModal);
    _fileHandlerProgressDialog->open();
    connect(_fileHandlerProgressDialog, &QProgressDialog::canceled, _fileHandler, [this](){_fileHandler->requestInterruption();});
}

void MainWindow::onFileHandlerProgressStepChanged(const QString &text) {
    _fileHandlerProgressDialog->setLabelText(text);
}

void MainWindow::onFileHandlerProgressMaximum(const int &maximum) {
    _fileHandlerProgressDialog->setMaximum(maximum);
}

void MainWindow::onFileHandlerProgressUpdate(const int &current) {
    _fileHandlerProgressDialog->setValue(current);
}

void MainWindow::onFileHandlerFinished() {
    // TODO: Handle if an error occured
    _projectData->setParent(this);
    _fileHandlerProgressDialog->close();
    _fileHandlerProgressDialog->deleteLater();
    _fileHandlerProgressDialog = nullptr;
    // TODO: reset all models
    if(!_workspaceHandler->onProjectCloseWorkspace())
        return;

    if(_workspaceHandler->currentWorkspace()->id() == _workspaceHandler->onProjectCloseWorkspace()->id())
        _workspaceHandler->switchToOnProjectOpenWorkspace();
}

void MainWindow::on_actionDebugGeneralTestAction_triggered() {
    #ifndef QT_DEBUG
    return;
    #endif

    qDebug() << "Test action triggered!";

    // test anything here!


    qDebug() << "finished!";
}

void MainWindow::on_actionDebugSimulateCrash_triggered() {
    #ifndef QT_DEBUG
        return;
    #endif

    QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Simulate crash"), tr("<p><b>Do you really want to crash the application now?</b></p><p>All unsaved work will be lost!</p>"), QMessageBox::Yes | QMessageBox::No);
    if(msg != QMessageBox::Yes)
        return;

    qWarning() << "Simulating crash...";
    int *p = nullptr;
    *p = 0;
}

void MainWindow::showGlobalSearch() {
    _globalSearch->open();
}
