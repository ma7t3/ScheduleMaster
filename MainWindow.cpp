#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Dialogs/DlgPreferences.h"
#include "Global/ActionShortcutMapper.h"
#include "Global/CrashDetector.h"
#include "Global/DockController.h"
#include "Global/FolderLocationManager.h"
#include "Global/LastUsedFilesManager.h"
#include "Global/Logger.h"
#include "Global/ProjectFileHandler.h"
#include "Global/Workspace.h"
#include "Global/WorkspaceHandler.h"
#include "MainWindowInterface.h"
#include "ProjectData/ProjectData.h"

#include <QDesktopServices>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QToolBar>
#include <QUndoStack>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _dockHandler(nullptr),
    _workspaceHandler(new WorkspaceHandler(this)),
    _projectData(new ProjectData(this)),
    _fileHandler(new ProjectFileHandler(_projectData, this)) {
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

    _undoAction->setIcon(QIcon(":/Icons/undo.ico"));
    _redoAction->setIcon(QIcon(":/Icons/redo.ico"));


    qDebug() << "   Loading shortcuts...";

    ActionShortcutMapper::map(ui->actionFileNewProject,      "project.new");
    ActionShortcutMapper::map(ui->actionFileOpenProject,     "project.open");
    ActionShortcutMapper::map(ui->actionFileSaveProject,     "project.save");
    ActionShortcutMapper::map(ui->actionFileSaveProjectAs,   "project.saveAs");
    ActionShortcutMapper::map(ui->actionFileCloseProject,    "project.close");
    ActionShortcutMapper::map(ui->actionFileQuit,            "application.quit");

    ActionShortcutMapper::map(ui->actionEditPreferences,     "application.preferences.open");
    ActionShortcutMapper::map(ui->actionEditProjectSettings, "project.settings.open");
    ActionShortcutMapper::map(ui->actionEditConfiguration,   "application.configuration.open");

    ActionShortcutMapper::map(_undoAction,                   "edit.undo");
    ActionShortcutMapper::map(_redoAction,                   "edit.redo");

    ui->menuEdit->insertAction(ui->actionEditPreferences, _undoAction);
    ui->menuEdit->insertAction(ui->actionEditPreferences, _redoAction);
    ui->menuEdit->insertSeparator(ui->actionEditPreferences);

    initToolbars();
    loadDocks();

    _workspaceHandler->workspace("home")->activate();

    updateRecentProjectsList();

    connect(ui->actionFileNewProject,         &QAction::triggered,                         this, &MainWindow::newProject);
    connect(ui->actionFileOpenProject,        &QAction::triggered,                         this, &MainWindow::openProject);
    connect(ui->actionFileSaveProject,        &QAction::triggered,                         this, &MainWindow::saveProject);
    connect(ui->actionFileSaveProjectAs,      &QAction::triggered,                         this, &MainWindow::saveProjectAs);
    connect(ui->actionFileCloseProject,       &QAction::triggered,                         this, &MainWindow::closeProject);
    connect(ui->actionFileQuit,               &QAction::triggered,                         this, &MainWindow::quitApplication);

    connect(ui->actionEditPreferences,        &QAction::triggered,                         this, &MainWindow::openPreferences);
    connect(ui->actionEditConfiguration,      &QAction::triggered,                         this, &MainWindow::openConfiguration);
    connect(ui->actionEditProjectSettings,    &QAction::triggered,                         this, &MainWindow::openProjectSettings);

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

void MainWindow::connectToInterface() {
    connect(MainWindowInterface::instance(), &MainWindowInterface::newProject,                  this, &MainWindow::newProject);
    connect(MainWindowInterface::instance(), &MainWindowInterface::openProject,                 this, &MainWindow::openProject);
    connect(MainWindowInterface::instance(), &MainWindowInterface::openProjectFromFile,         this, &MainWindow::openProjectFromFile);
    connect(MainWindowInterface::instance(), &MainWindowInterface::saveProject,                 this, &MainWindow::saveProject);
    connect(MainWindowInterface::instance(), &MainWindowInterface::saveProjectAs,               this, &MainWindow::saveProjectAs);
    connect(MainWindowInterface::instance(), &MainWindowInterface::closeProject,                this, &MainWindow::closeProject);
    connect(MainWindowInterface::instance(), &MainWindowInterface::quitApplication,             this, &MainWindow::quitApplication);
    connect(MainWindowInterface::instance(), &MainWindowInterface::removeProjectFromRecentList, this, &MainWindow::removeProjectFromRecentList);
    connect(MainWindowInterface::instance(), &MainWindowInterface::openPlugins,                 this, &MainWindow::openPlugins);
    connect(MainWindowInterface::instance(), &MainWindowInterface::openPreferences,             this, &MainWindow::openPreferences);
    connect(MainWindowInterface::instance(), &MainWindowInterface::openConfiguration,           this, &MainWindow::openConfiguration);
    connect(MainWindowInterface::instance(), &MainWindowInterface::openProjectSettings,         this, &MainWindow::openProjectSettings);
}

void MainWindow::loadDocks() {
    qInfo() << "   Loading docks...";
    _dockHandler = new DockController(this);
    connect(_dockHandler, &DockController::dockAdded, this, &MainWindow::onDockAdded);
    _dockHandler->loadStandardDocks();
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

        QAction *action = menu->addAction(QString::number(i) + ": " + path);
        connect(action, &QAction::triggered, this, [this, path](){openProjectFromFile(path);});
        i++;
    }
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

void MainWindow::newProject() {
    closeProject();
    qInfo() << "Create new project...";
}

void MainWindow::openProject() {
    const QString path = QFileDialog::getOpenFileName(this, tr("Open Project File"), FolderLocationManager::currentFolderLocationPaths("projectFilesDefault").first(), tr("ScheduleMaster Project File (*.smp);;JSON (*.json)"));
    if(path.isEmpty())
        return;

    openProjectFromFile(path);
}

void MainWindow::openProjectFromFile(const QString &filePath) {
    closeProject();
    qInfo() << "Open project file" << filePath;
    createFileHandlerProgressDialog(tr("Open project..."));
    _fileHandler->readFile(filePath);
}

void MainWindow::saveProject() {
    qInfo() << "Save project";
}

void MainWindow::saveProjectAs() {
    qInfo() << "Save project as";
}

void MainWindow::saveProjectToFile(const QString &filePath) {
    qInfo() << "Save project to file" << filePath;
}

void MainWindow::closeProject() {
    qInfo().noquote() << "Closing project" << _projectData->filePath();
    //TODO: Check for unsaved changes
    _projectData->reset();
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
