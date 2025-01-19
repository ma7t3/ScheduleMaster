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

    _dwWelcome->setWidget(_wdgWelcome);

    addDockWidget(Qt::TopDockWidgetArea, _dwWelcome);
}
