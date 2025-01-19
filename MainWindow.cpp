#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->centralwidget->hide();

    initToolbars();

    ui->menuEdit->addAction(ui->actionEditPreferences);
    ui->menuEdit->addAction(ui->actionEditProjectSettings);

    initDockWidgets();

    connect(ui->actionFileNewProject,      &QAction::triggered, this, &MainWindow::newProject);
    connect(ui->actionFileOpenProject,     &QAction::triggered, this, &MainWindow::openProject);
    connect(ui->actionFileSaveProject,     &QAction::triggered, this, &MainWindow::saveProject);
    connect(ui->actionFileSaveProjectAs,   &QAction::triggered, this, &MainWindow::saveProjectAs);
    connect(ui->actionFileCloseProject,    &QAction::triggered, this, &MainWindow::closeProject);
    connect(ui->actionFileQuit,            &QAction::triggered, this, &MainWindow::quitApplication);

    connect(ui->actionEditPreferences,     &QAction::triggered, this, &MainWindow::openPreferences);
    connect(ui->actionEditProjectSettings, &QAction::triggered, this, &MainWindow::openProjectSettings);
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
    // undo/redo
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
    connect(_wdgWelcome, &WdgWelcome::newProject,          this, &MainWindow::newProject);
    connect(_wdgWelcome, &WdgWelcome::openProject,         this, &MainWindow::openProject);
    connect(_wdgWelcome, &WdgWelcome::openProjectFromFile, this, &MainWindow::openProjectFromFile);
    connect(_wdgWelcome, &WdgWelcome::openPreferences,     this, &MainWindow::openPreferences);
    connect(_wdgWelcome, &WdgWelcome::quitApplication,     this, &MainWindow::quitApplication);

    connect(_wdgWelcome, &WdgWelcome::removeProjectFromList,     this, [](){qInfo() << "test";});



    _dwWelcome->setWidget(_wdgWelcome);

    addDockWidget(Qt::TopDockWidgetArea, _dwWelcome);
}

void MainWindow::newProject() {
    qDebug() << "new project";
}

void MainWindow::openProject() {
    qDebug() << "open project with dialog";
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

void MainWindow::openPreferences() {
    qDebug() << "open preferences";
}

void MainWindow::openProjectSettings() {
    qDebug() << "open project settings";
}

