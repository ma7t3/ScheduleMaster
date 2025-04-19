#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMessageBox>
#include <QFileDialog>

#include "Global/LocalConfig.h"
#include "Global/ActionShortcutMapper.h"
#include "Dialogs/DlgPreferences.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _workspaceHandler(nullptr),
    _projectData(new ProjectData(this)) {
    ui->setupUi(this);

    qInfo() << "Loading MainWindow";

    ui->centralwidget->hide();

#ifndef QT_DEBUG
    ui->menuDebug->setHidden(true);
#endif

    qDebug() << "   Loading Undo/redo actions...";
    _undoAction = _projectData->undoStack()->createUndoAction(this, tr("Undo"));
    _redoAction = _projectData->undoStack()->createRedoAction(this, tr("Redo"));

    _undoAction->setIcon(QIcon(":/Icons/Undo.ico"));
    _redoAction->setIcon(QIcon(":/Icons/Redo.ico"));


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

    loadWorkspaces();

    initToolbars();

    initDockWidgets();

    updateRecentProjectsList();

    connect(ui->actionFileNewProject,      &QAction::triggered,                this,        &MainWindow::newProject);
    connect(ui->actionFileOpenProject,     &QAction::triggered,                this,        &MainWindow::openProject);
    connect(ui->actionFileSaveProject,     &QAction::triggered,                this,        &MainWindow::saveProject);
    connect(ui->actionFileSaveProjectAs,   &QAction::triggered,                this,        &MainWindow::saveProjectAs);
    connect(ui->actionFileCloseProject,    &QAction::triggered,                this,        &MainWindow::closeProject);
    connect(ui->actionFileQuit,            &QAction::triggered,                this,        &MainWindow::quitApplication);

    connect(ui->actionEditPreferences,     &QAction::triggered,                this,        &MainWindow::openPreferences);
    connect(ui->actionEditConfiguration,   &QAction::triggered,                this,        &MainWindow::openConfiguration);
    connect(ui->actionEditProjectSettings, &QAction::triggered,                this,        &MainWindow::openProjectSettings);

    connect(LocalConfig::instance(),       &LocalConfig::lastUsedFilesChanged, this,        &MainWindow::updateRecentProjectsList);
    connect(LocalConfig::instance(),       &LocalConfig::lastUsedFilesChanged, _wdgWelcome, &WdgWelcome::updateRecentProjectsList);
    connect(LocalConfig::instance(),       &LocalConfig::uiFontChanged,        this,        &MainWindow::setUiFont);
    connect(LocalConfig::instance(),       &LocalConfig::accentColorChanged,   this,        &MainWindow::setAccentColor);

    LocalConfig::previewUiFontFamily();

    showCrashWarning();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::loadWorkspaces() {
    qInfo() << "   Loading workspaces...";
    _workspaceHandler = new WorkspaceHandler(this);
    _workspaceHandler->setWorkspacesMenu(ui->menuWorkspaces);
    _workspaceHandler->workspace(WorkspaceHandler::HomeWorkspace)->activate();
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

void MainWindow::initDockWidgets() {
    qInfo() << "Initializing docks";
    _dwWelcome = new QDockWidget(tr("Welcome"), this);
    _dwWelcome->setObjectName("dwWelcome");

    _wdgWelcome = new WdgWelcome(this);
    connect(_wdgWelcome, &WdgWelcome::newProject,            this, &MainWindow::newProject);
    connect(_wdgWelcome, &WdgWelcome::openProject,           this, &MainWindow::openProject);
    connect(_wdgWelcome, &WdgWelcome::openProjectFromFile,   this, &MainWindow::openProjectFromFile);
    connect(_wdgWelcome, &WdgWelcome::openPlugins,           this, &MainWindow::openPlugins);
    connect(_wdgWelcome, &WdgWelcome::openPreferences,       this, &MainWindow::openPreferences);
    connect(_wdgWelcome, &WdgWelcome::quitApplication,       this, &MainWindow::quitApplication);

    connect(_wdgWelcome, &WdgWelcome::removeProjectFromList, this, &MainWindow::removeProjectFromRecentList);

    _dwWelcome->setWidget(_wdgWelcome);

    addDockWidget(Qt::TopDockWidgetArea, _dwWelcome);
}

void MainWindow::updateRecentProjectsList() {
    qInfo() << "Updating open recent projects list...";

    QMenu *menu = ui->menuFile->findChild<QMenu *>("menuFileOpenRecent");
    if(!menu)
        return;

    menu->clear();

    QStringList list = LocalConfig::lastUsedFiles();

    int i = 0;
    for(QString path : list) {
        if(i > 9)
            return;

        QAction *action = menu->addAction(QString::number(i) + ": " + path);
        connect(action, &QAction::triggered, this, [this, path](){openProjectFromFile(path);});
        i++;
    }
}

void MainWindow::showCrashWarning() {
    if(LocalConfig::crashDetected()) {
        bool logfileSaved = QFile::exists(LocalConfig::lastLogfileName());

        qInfo() << "crash detected" + (logfileSaved ? ", logfile saved separately: " + LocalConfig::lastLogfileName() : "");
        qInfo().noquote() << "crash detected" + (logfileSaved ? ", logfile saved separately: " + LocalConfig::lastLogfileName() : "");
        QString messageTitle = tr("Crash detected");
        QString messageStr = tr("<p><b>Seems like ScheduleMaster crashed last time you used it</b></p><p>If this was unexpected (e.g. you didn't try to kill the process via the Windows Task Manager or something), please feel free to send a bug report!</p>");
        QString messageStrAddition = tr("<p>The logfile of your last session was saved seperately. Do you want to open it?</p>");
        qApp->restoreOverrideCursor();
        if(QFile::exists(LocalConfig::lastLogfileName())) {
            messageStr += messageStrAddition;
            QMessageBox::StandardButton msg = QMessageBox::warning(this, messageTitle, messageStr, QMessageBox::Yes|QMessageBox::No);
            if(msg == QMessageBox::Yes) {
                QDesktopServices::openUrl(LocalConfig::lastLogfileName());
            }
        } else {
            QMessageBox::warning(this, messageTitle, messageStr, QMessageBox::Ok);
        }
        qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    }
    LocalConfig::setCrashDetected(true);
}

void MainWindow::newProject() {
    closeProject();
    qInfo() << "Create new project...";
}

void MainWindow::openProject() {
    QString path = QFileDialog::getOpenFileName(this, tr("Open Project File"), LocalConfig::folderLocationPaths("base.projectFilesDefault").first(), tr("ScheduleMaster Project File (*.smp);;JSON (*.json)"));
    if(path.isEmpty())
        return;

    openProjectFromFile(path);
}

void MainWindow::openProjectFromFile(const QString &filePath) {
    qInfo() << "Open project file" << filePath;
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
    qInfo() << "Close project";
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

    LocalConfig::removeLastUsedFile(filePath);
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


void MainWindow::setUiFont(const QString &fontFamily) {
    QFont font = qApp->font();
    font.setFamily(fontFamily);
    qApp->setFont(font);
}

void MainWindow::setAccentColor(const QString &id) {
    const QColor color = GlobalConfig::accentColor(id);
    QPalette palette = QApplication::palette();
    if(color.isValid()) {
        palette.setColor(QPalette::Highlight, color);
        QApplication::setPalette(palette);
    } else {
        QApplication::setPalette(QApplication::style()->standardPalette());
    }
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
