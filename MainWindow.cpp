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

    connect(ui->actionFileQuit, &QAction::triggered, this, &MainWindow::quitApplication);

    connect(ui->actionEditPreferences, &QAction::triggered, this, &MainWindow::openPreferences);
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
    connect(_wdgWelcome, &WdgWelcome::openProjectFromFile, this, &MainWindow::openProject);
    connect(_wdgWelcome, &WdgWelcome::openPreferences,     this, &MainWindow::openPreferences);
    connect(_wdgWelcome, &WdgWelcome::quitApplication,     this, &MainWindow::quitApplication);

    _dwWelcome->setWidget(_wdgWelcome);

    addDockWidget(Qt::TopDockWidgetArea, _dwWelcome);
}

void MainWindow::newProject() {}

void MainWindow::openProject() {}

void MainWindow::openProjectFromFile(const QString &filePath) {}

void MainWindow::saveProject() {}

void MainWindow::saveProjectAs() {}

void MainWindow::saveProjectToFile(const QString &filePath) {}

void MainWindow::closeProject() {}

void MainWindow::openPreferences() {
    qInfo() << "open prefs";
}

void MainWindow::quitApplication() {
    QApplication::quit();
}
