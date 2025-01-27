#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMessageBox>
#include <QFileDialog>

#include "Global/LocalConfig.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _projectData(new ProjectData(this)) {
    ui->setupUi(this);

    ui->centralwidget->hide();

#ifndef QT_DEBUG
    ui->menuDebug->setHidden(true);
#endif

    _undoAction = _projectData->undoStack()->createUndoAction(this, tr("Undo"));
    _redoAction = _projectData->undoStack()->createRedoAction(this, tr("Redo"));

    _undoAction->setShortcut(QKeySequence(QKeySequence::Undo));
    _redoAction->setShortcut(QKeySequence(QKeySequence::Redo));

    _undoAction->setIcon(QIcon(":/Icons/Undo.ico"));
    _redoAction->setIcon(QIcon(":/Icons/Redo.ico"));

    ui->menuEdit->insertAction(ui->actionEditPreferences, _undoAction);
    ui->menuEdit->insertAction(ui->actionEditPreferences, _redoAction);
    ui->menuEdit->insertSeparator(ui->actionEditPreferences);

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
    connect(ui->actionEditProjectSettings, &QAction::triggered,                this,        &MainWindow::openProjectSettings);

    connect(LocalConfig::instance(),       &LocalConfig::lastUsedFilesChanged, this,        &MainWindow::updateRecentProjectsList);
    connect(LocalConfig::instance(),       &LocalConfig::lastUsedFilesChanged, _wdgWelcome, &WdgWelcome::updateRecentProjectsList);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initToolbars() {
    _toolbarGeneral    = new QToolBar(this);
    _toolbarDocks      = new QToolBar(this);
    _toolbarWorkspaces = new QToolBar(this);

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
    _toolbarWorkspaces->addAction(ui->actionViewWorkspaceHome);
    _toolbarWorkspaces->addAction(ui->actionViewWorkspaceRouting);
    _toolbarWorkspaces->addAction(ui->actionViewWorkspaceScheduling);
    _toolbarWorkspaces->addAction(ui->actionViewWorkspaceTours);
    _toolbarWorkspaces->addAction(ui->actionViewWorkspacePublish);

    addToolBar(_toolbarGeneral);
    addToolBar(_toolbarDocks);
    addToolBar(_toolbarWorkspaces);
}

void MainWindow::initDockWidgets() {
    _dwWelcome = new QDockWidget(tr("Welcome"), this);

    _wdgWelcome = new WdgWelcome(this);
    connect(_wdgWelcome, &WdgWelcome::newProject,            this, &MainWindow::newProject);
    connect(_wdgWelcome, &WdgWelcome::openProject,           this, &MainWindow::openProject);
    connect(_wdgWelcome, &WdgWelcome::openProjectFromFile,   this, &MainWindow::openProjectFromFile);
    connect(_wdgWelcome, &WdgWelcome::openPreferences,       this, &MainWindow::openPreferences);
    connect(_wdgWelcome, &WdgWelcome::quitApplication,       this, &MainWindow::quitApplication);

    connect(_wdgWelcome, &WdgWelcome::removeProjectFromList, this, &MainWindow::removeProjectFromRecentList);

    _dwWelcome->setWidget(_wdgWelcome);

    addDockWidget(Qt::TopDockWidgetArea, _dwWelcome);
}

void MainWindow::updateRecentProjectsList() {
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

void MainWindow::newProject() {
    closeProject();
    qInfo() << "create new project...";
}

void MainWindow::openProject() {
    QString path = QFileDialog::getOpenFileName(this, tr("Open Project File"), LocalConfig::defaultProjectLocation(), tr("ScheduleMaster Project File (*.smp);;JSON (*.json)"));
    if(path.isEmpty())
        return;

    openProjectFromFile(path);
}

void MainWindow::openProjectFromFile(const QString &filePath) {
    qDebug() << "open project file" << filePath;
}

void MainWindow::saveProject() {
    qDebug() << "save project";
}

void MainWindow::saveProjectAs() {
    qDebug() << "save project as";
}

void MainWindow::saveProjectToFile(const QString &filePath) {
    qDebug() << "save project to file" << filePath;
}

void MainWindow::closeProject() {
    qDebug() << "close project";
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

void MainWindow::openPreferences() {
    qDebug() << "open preferences";
}

void MainWindow::openProjectSettings() {
    qDebug() << "open project settings";
}

void MainWindow::on_actionDebugGeneralTestAction_triggered() {
    #ifndef QT_DEBUG
    return;
    #endif

    qDebug() << "Test action triggered!";

    // test anything here!
}
