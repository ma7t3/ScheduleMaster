//********************************************************************************************************************************************************************
// Includes
//********************************************************************************************************************************************************************


#include <QCloseEvent>

#include <QSplashScreen>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QToolBar>

#include <QUuid>
#include <QScrollBar>

#include <QPdfWriter>
#include <QPainter>
#include <QDesktopServices>


#include "Commands/Cmdgeneral.h"
#include "Mainwindow.h"

#include "App/global.h"
#include "localconfig.h"
#include "globalconfig.h"
#include "ui_Mainwindow.h"

#include "Widgets/wdgschedule.h"

#include "Dialogs/DlgStartupdialog.h"
#include "Dialogs/DlgProjecttreeviewer.h"
#include "Dialogs/DlgTroubleshooter.h"
#include "Dialogs/DlgProjectsettings.h"
#include "Dialogs/DlgPreferences.h"
#include "Dialogs/DlgManageFootnotes.h"


//********************************************************************************************************************************************************************
// Constructor, Destructor, Overwrites (CloseEvent e.g.)
//********************************************************************************************************************************************************************

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    projectData(new ProjectData),
    undoStack(new QUndoStack),
    startupDialog(new StartupDialog),
    fileHandler(new DlgFileHandler(this, projectData)),
    pdfExporter(new DlgPdfExporter(this, projectData)),
    knownFile(false)
{
    if(LocalConfig::crashDetected()) {
        QString messageTitle = tr("Crash detected");
        QString messageStr = tr("<p><b>Seems like ScheduleMaster crashed last time you used it</b></p><p>If this was unexpected (e.g. you didn't try to kill the process via the Windows Task Manager or something), please feel free to send a bug report!</p>");
        QString messageStrAddition = tr("<p>The logfile of your last session was saved seperately. Do you want to open it?</p>");
        if(QFile::exists(LocalConfig::lastLogfileName())) {
            messageStr += messageStrAddition;
            QMessageBox::StandardButton msg = QMessageBox::warning(this, messageTitle, messageStr, QMessageBox::Yes|QMessageBox::No);
            if(msg == QMessageBox::Yes) {
                QDesktopServices::openUrl(LocalConfig::lastLogfileName());
            }
        } else {
            QMessageBox::warning(this, messageTitle, messageStr, QMessageBox::Ok);
        }
    }

    LocalConfig::setCrashDetected(true);

    qInfo() << "initializing splashscreen...";

    // init splashscreen
    QSplashScreen splashScreen;
    QString imagePath = ":/splashscreen/splashscreen.png";
    QColor messageColor = QColor(0, 0, 0);
    QDate now = QDate::currentDate();

    if(now.dayOfYear() > 300 && now.dayOfYear() < 310)
        imagePath = ":/splashscreen/splashscreen_halloween.png";

    if(now.dayOfYear() > 330 && now.dayOfYear() < 365) {
        imagePath = ":/splashscreen/splashscreen_christmas.png";
        messageColor = QColor(255, 255, 255);
    }

    QPixmap pixmap(imagePath);
    splashScreen.setPixmap(pixmap);
    splashScreen.showMessage(tr("loading ui..."), Qt::AlignBottom, messageColor);
    //splashScreen.finish(startupDialog);
    splashScreen.show();
    QObject().thread()->sleep(2);

    qInfo() << "loading user interface...";
    // load ui
    ui->setupUi(this);
    MainWindow::hide();

    ui->centralwidget->setHidden(true);

    dwUndoView->setWidget(wdgUndoView);
    dwBusstops->setWidget(wdgBusstops);
    dwLines->setWidget(wdgLines);
    dwRoutes->setWidget(wdgRoutes);
    dwSchedule->setWidget(wdgSchedule);
    dwTripEditor->setWidget(wdgTripEditor);
    dwBusstopSchedule->setWidget(wdgBusstopSchedule);
    dwTours->setWidget(wdgTours);
    dwTourEditor->setWidget(wdgTourEditor);
    dwPublishedLines->setWidget(wdgPublishedLines);

    actionWorkspaceTrackLayout();

    qInfo() << "loading menubar...";
    // load menubar
    splashScreen.showMessage(tr("loading menubar..."), Qt::AlignBottom, messageColor);

    QAction *actDockBusstops = dwBusstops->toggleViewAction();
    QAction *actDockLines= dwLines->toggleViewAction();
    QAction *actDockRoutes = dwRoutes->toggleViewAction();
    QAction *actDockSchedule = dwSchedule->toggleViewAction();
    QAction *actDockBusstopSchedule = dwBusstopSchedule->toggleViewAction();
    QAction *actDockTours = dwTours->toggleViewAction();
    QAction *actDockTourEditor = dwTourEditor->toggleViewAction();
    QAction *actDockPublishedLines = dwPublishedLines->toggleViewAction();

    actDockBusstops->setIcon(QIcon(":/icons/Busstop.ico"));
    actDockLines->setIcon(QIcon(":/icons/Line.ico"));
    actDockRoutes->setIcon(QIcon(":/icons/Route.ico"));
    actDockSchedule->setIcon(QIcon(":/icons/Schedule.ico"));
    actDockBusstopSchedule->setIcon(QIcon(":/icons/BusstopSchedule.ico"));
    actDockTours->setIcon(QIcon(":/icons/TourList.ico"));
    actDockTourEditor->setIcon(QIcon(":/icons/Tour.ico"));
    actDockPublishedLines->setIcon(QIcon(":/icons/PublishedLines.ico"));

    actDockBusstops->setShortcut(QKeySequence(tr("Alt+B")));
    actDockLines->setShortcut(QKeySequence(tr("Alt+L")));
    actDockRoutes->setShortcut(QKeySequence(tr("Alt+R")));
    actDockSchedule->setShortcut(QKeySequence(tr("Alt+S")));
    actDockBusstopSchedule->setShortcut(QKeySequence(tr("Alt+Shift+B")));
    actDockTours->setShortcut(QKeySequence(tr("Alt+T")));
    actDockTourEditor->setShortcut(QKeySequence(tr("Alt+Shift+T")));

    ui->actionFileNew->setShortcut(QKeySequence::New);
    ui->actionFileOpen->setShortcut(QKeySequence::Open);
    ui->actionFileSave->setShortcut(QKeySequence::Save);
    ui->actionFileSaveAs->setShortcuts({QKeySequence(Qt::CTRL|Qt::SHIFT|Qt::Key_S), QKeySequence::SaveAs});
    ui->actionFileClose->setShortcuts({QKeySequence(Qt::CTRL|Qt::Key_W), QKeySequence::Close});
    ui->actionFileQuit->setShortcuts({QKeySequence(Qt::CTRL|Qt::Key_Q), QKeySequence::Quit});

    ui->actionUndo->setShortcut(QKeySequence::Undo);
    ui->actionRedo->setShortcut(QKeySequence::Redo);
    ui->actionEditProjectSettings->setShortcut(QKeySequence(Qt::CTRL|Qt::SHIFT|Qt::Key_Comma));
    ui->actionEditPreferences->setShortcuts({QKeySequence(Qt::CTRL|Qt::Key_Comma), QKeySequence::Preferences});


    ui->menuDocks->addAction(actDockBusstops);
    ui->menuDocks->addAction(actDockLines);
    ui->menuDocks->addAction(actDockRoutes);
    ui->menuDocks->addAction(actDockSchedule);
    ui->menuDocks->addAction(actDockBusstopSchedule);
    ui->menuDocks->addAction(actDockTours);
    ui->menuDocks->addAction(actDockTourEditor);
    ui->menuDocks->addSeparator();
    ui->menuDocks->addAction(actDockPublishedLines);

    wdgBusstops->setMenubarActions(ui->actionBusstopsNew, ui->actionBusstopsEdit, ui->actionBusstopsDelete);
    wdgLines->setMenubarActions(ui->actionLinesNew, ui->actionLinesEdit, ui->actionLinesDelete);
    wdgRoutes->setMenubarActions(ui->actionRoutesNew, ui->actionRoutesEdit, ui->actionRoutesDuplicate, ui->actionRoutesDelete);

    connect(wdgSchedule, SIGNAL(currentLineChanged(Line*,LineDirection*)), wdgTripEditor, SLOT(setCurrentLine(Line*,LineDirection*)));
    connect(wdgSchedule, SIGNAL(currentDayTypeChanged(DayType)), wdgTripEditor, SLOT(setCurrentDayType(DayType)));
    connect(wdgSchedule, SIGNAL(currentTripsChanged(QList<Trip*>)), wdgTripEditor, SLOT(setCurrentTrips(QList<Trip*>)));
    connect(wdgTripEditor, SIGNAL(tripsChanged(QList<Trip*>)), wdgSchedule, SLOT(refreshSchedule()));


    connect(ui->actionBusstopsNew, SIGNAL(triggered()), wdgBusstops, SLOT(actionNew()));
    connect(ui->actionBusstopsEdit, SIGNAL(triggered()), wdgBusstops, SLOT(actionEdit()));
    connect(ui->actionBusstopsDelete, SIGNAL(triggered()), wdgBusstops, SLOT(actionDelete()));

    connect(ui->actionLinesNew, SIGNAL(triggered()), wdgLines, SLOT(actionNew()));
    connect(ui->actionLinesEdit, SIGNAL(triggered()), wdgLines, SLOT(actionEdit()));
    connect(ui->actionLinesDelete, SIGNAL(triggered()), wdgLines, SLOT(actionDelete()));

    connect(ui->actionRoutesNew, SIGNAL(triggered()), wdgRoutes, SLOT(actionNew()));
    connect(ui->actionRoutesEdit, SIGNAL(triggered()), wdgRoutes, SLOT(actionEdit()));
    connect(ui->actionRoutesDuplicate, SIGNAL(triggered()), wdgRoutes, SLOT(actionDuplicate()));
    connect(ui->actionRoutesDelete, SIGNAL(triggered()), wdgRoutes, SLOT(actionDelete()));

    connect(ui->actionScheduleAddTrip, SIGNAL(triggered()), wdgTripEditor, SLOT(actionNew()));
    connect(ui->actionScheduleCopyTrip, SIGNAL(triggered()), wdgTripEditor, SLOT(actionCopy()));
    connect(ui->actionScheduleDeleteSelectedTrips, SIGNAL(triggered()), wdgTripEditor, SLOT(actionDelete()));

    tbGeneral = new QToolBar(tr("general"), this);
    tbGeneral->setToolButtonStyle(Qt::ToolButtonIconOnly);
    tbGeneral->addAction(ui->actionFileNew);
    tbGeneral->addAction(ui->actionFileOpen);
    tbGeneral->addAction(ui->actionFileSave);
    tbGeneral->addAction(ui->actionUndo);
    tbGeneral->addAction(ui->actionRedo);
    tbGeneral->addSeparator();
    tbGeneral->addAction(ui->actionEditProjectSettings);
    tbGeneral->addAction(ui->actionEditPreferences);

    tbDocks = new QToolBar(tr("docks"), this);
    tbDocks->setToolButtonStyle(Qt::ToolButtonIconOnly);
    tbDocks->addAction(actDockBusstops);
    tbDocks->addAction(actDockLines);
    tbDocks->addAction(actDockRoutes);
    tbDocks->addAction(actDockSchedule);
    tbDocks->addAction(actDockBusstopSchedule);
    tbDocks->addAction(actDockTours);
    tbDocks->addAction(actDockTourEditor);
    tbDocks->addAction(actDockPublishedLines);

    tbWorkspaces = new QToolBar(tr("workspaces"), this);
    tbWorkspaces->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tbWorkspaces->addAction(ui->actionWorkspaceTrackLayout);
    tbWorkspaces->addAction(ui->actionWorkspaceBusstopSchedule);
    tbWorkspaces->addAction(ui->actionWorkspaceScheduling);
    tbWorkspaces->addAction(ui->actionWorkspaceTourPlanning);
    tbWorkspaces->addAction(ui->actionWorkspacePublish);

    this->addToolBar(Qt::TopToolBarArea, tbGeneral);
    this->addToolBar(Qt::LeftToolBarArea, tbDocks);
    this->addToolBar(Qt::TopToolBarArea, tbWorkspaces);

    ui->actionViewToolbarGeneral->setChecked(true);
    ui->actionViewToolbarDocks->setChecked(true);
    ui->actionViewToolbarWorkspaces->setChecked(true);

    dwBusstopSchedule->resize(900, 831);

    setRedoEnabled(false);
    setUndoEnabled(false);
    setSaved(true);

    qInfo() << "loading signals and slots...";
    // load signals and slots
    splashScreen.showMessage(tr("loading signals and slots..."), Qt::AlignBottom, messageColor);

    connect(ui->actionFileNew, SIGNAL(triggered()), this, SLOT(actionFileNew()));
    connect(ui->actionFileOpen, SIGNAL(triggered()), this, SLOT(actionFileOpen()));
    connect(ui->actionFileSave, SIGNAL(triggered()), this, SLOT(actionFileSave()));
    connect(ui->actionFileSaveAs, SIGNAL(triggered()), this, SLOT(actionFileSaveAs()));
    connect(ui->actionFileClose, SIGNAL(triggered()), this, SLOT(actionFileClose()));
    connect(ui->actionFileQuit, SIGNAL(triggered()), this, SLOT(actionQuit()));

    connect(undoStack, SIGNAL(cleanChanged(bool)), this, SLOT(setSaved(bool)));
    connect(undoStack, SIGNAL(canUndoChanged(bool)), this, SLOT(setUndoEnabled(bool)));
    connect(undoStack, SIGNAL(canRedoChanged(bool)), this, SLOT(setRedoEnabled(bool)));
    connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(actionEditUndo()));
    connect(ui->actionRedo, SIGNAL(triggered()), this, SLOT(actionEditRedo()));

    connect(ui->actionWorkspaceTrackLayout, SIGNAL(triggered()), this, SLOT(actionWorkspaceTrackLayout()));
    connect(ui->actionWorkspaceBusstopSchedule, SIGNAL(triggered()), this, SLOT(actionWorkspaceBusstopSchedule()));
    connect(ui->actionWorkspaceScheduling, SIGNAL(triggered()), this, SLOT(actionWorkspaceScheduling()));
    connect(ui->actionWorkspaceTourPlanning, SIGNAL(triggered()), this, SLOT(actionWorkspaceTourPlanning()));
    connect(ui->actionWorkspacePublish, SIGNAL(triggered()), this, SLOT(actionWorkspacePublish()));

    connect(wdgBusstops, SIGNAL(currentBusstopChanged(Busstop*)), wdgBusstopSchedule, SLOT(setBusstop(Busstop*)));

    connect(wdgLines, SIGNAL(currentLineChanged(Line*)), wdgRoutes, SLOT(setCurrentLine(Line*)));
    connect(wdgLines, SIGNAL(currentLineChanged(Line*)), wdgSchedule, SLOT(setCurrentLine(Line*)));

    connect(wdgSchedule, SIGNAL(busstopScheduleRequested(Busstop *, QList<Route *>, DayType *)), this, SLOT(actionOpenBusstopSchedule(Busstop *, QList<Route *>, DayType *)));
    connect(wdgSchedule, SIGNAL(tourRequested(Tour *)), this, SLOT(actionOpenTour(Tour *)));
    connect(wdgTours, SIGNAL(currentTourChanged(Tour*)), wdgTourEditor, SLOT(setCurrentTour(Tour*)));


    //              SENDER SIGNAL                              RECIVER      SLOT
    QObject::connect(this, &MainWindow::busstopsChanged,       wdgBusstops, &WdgBusstops::refresh);
    QObject::connect(this, &MainWindow::linesChanged,          wdgLines,    &WdgLines::refresh);
    QObject::connect(this, &MainWindow::routesChanged,         wdgRoutes,   &WdgRoutes::refresh);
    QObject::connect(this, &MainWindow::projectSettingsChanged,       wdgSchedule, &WdgSchedule::refreshDayTypes);
    QObject::connect(this, &MainWindow::lineDirectionsChanged, wdgSchedule, &WdgSchedule::refreshDirections);
    QObject::connect(this, &MainWindow::tripsChanged,          wdgSchedule, &WdgSchedule::refreshSchedule);


    QList<QAction *> actions;
    QStringList lastUsedFiles = LocalConfig::lastUsedFiles();

    foreach(QString path, lastUsedFiles) {
        QFileInfo fi(path);
        QAction *action = new QAction(fi.fileName(), this);
        actions << action;
    }

    ui->menuOpenRecent->addActions(actions);

    splashScreen.showMessage(tr("loading startup dialog..."), Qt::AlignBottom, messageColor);

    MainWindow::showMaximized();

    qInfo() << "loading startup dialog";

    splashScreen.close();
    startupDialog->exec();
    if(startupDialog->result() == QDialog::Accepted) {
        int action =startupDialog->getAction();
        if(action == StartupDialog::openFile)
            actionFileOpen();
        else if(action == StartupDialog::openRecentFile)
            openFile(startupDialog->getFilePath());
        else if(action == StartupDialog::quit) {
            delete startupDialog;
            MainWindow::close();
        }
    }

    delete startupDialog;
}

MainWindow::~MainWindow()
{
    LocalConfig::setCrashDetected(false);
    delete ui;
}

// Close Event
void MainWindow::closeEvent(QCloseEvent * event){
    if(!actionFileClose())
        event->ignore();
    else {
        qInfo() << "closing mainWindow...";
        event->accept();
    }
}


//********************************************************************************************************************************************************************
// Actions
//********************************************************************************************************************************************************************

bool MainWindow::actionFileNew() {
    return actionFileClose();
}

bool MainWindow::actionFileOpen() {
    if(!actionFileClose())
        return false;

    QDir dir = QDir::homePath() + "/.ScheduleMaster/Projects";
    if(!dir.exists())
        dir.mkpath(dir.path());

    QString path = QFileDialog::getOpenFileName(this, "", dir.path(), tr("ScheduleMaster File (*.smp  *.json)"));
    if(path == "") {
        qWarning() << "cannot open file \"" << path << "\" - invalid path";
        return false;
    }

    return openFile(path);
}

bool MainWindow::actionFileSave() {
    if(!knownFile)
        return actionFileSaveAs();

    return saveFile(projectData->filePath());
    //return saveFile(projectFilePath);
}

bool MainWindow::actionFileSaveAs() {
    QDir dir = QDir::homePath() + "/.ScheduleMaster/Projects";
    if(!dir.exists())
        dir.mkpath(dir.path());

    QString path = QFileDialog::getSaveFileName(this, "", dir.path(), tr("ScheduleMaster File (*.smp);;JSON File (*.json)"));
    if(path == "")
        return false;

    if(saveFile(path)) {
        knownFile = true;
        //projectFilePath = path;
        projectData->setFilePath(path);
        return true;
    }
    return false;
}

bool MainWindow::actionFileClose() {
    if(!undoStack->isClean()) {
        QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Unsaved Changes"), tr("<p><b>There are unsaved changes!</b></p><p>Do want to save them before closing this file?</p>"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

        if(msg == QMessageBox::Yes) {
            if(!actionFileSave())
                return false;
        } else if(msg == QMessageBox::Cancel) {
            return false;
        }
    }
    qInfo() << "closing current file...";
    projectData->reset();
    knownFile = false;
    //projectFilePath = "";
    projectData->setFilePath("");
    wdgBusstops->refresh();
    wdgLines->refresh();
    wdgRoutes->refresh();
    wdgSchedule->refreshSchedule();
    wdgTours->refreshTourList();
    wdgTourEditor->refreshTour();
    wdgPublishedLines->refreshDayTypes();
    wdgPublishedLines->refreshRouteList();
    return true;
}

bool MainWindow::actionQuit() {
    close();
    return true;
}

void MainWindow::actionEditUndo() {
    undoStack->undo();
    wdgBusstops->refresh();
    wdgLines->refresh();
    wdgRoutes->refresh();
    wdgSchedule->refreshSchedule();
    wdgTours->refreshTourList();
    wdgTourEditor->refreshTour();
    wdgSchedule->refreshDayTypes();
    wdgPublishedLines->refreshDayTypes();
    wdgPublishedLines->refreshRouteList();
}

void MainWindow::actionEditRedo() {
    undoStack->redo();
    wdgBusstops->refresh();
    wdgLines->refresh();
    wdgRoutes->refresh();
    wdgSchedule->refreshSchedule();
    wdgTours->refreshTourList();
    wdgTourEditor->refreshTour();
    wdgSchedule->refreshDayTypes();
    wdgPublishedLines->refreshDayTypes();
    wdgPublishedLines->refreshRouteList();
}

/* Notiz an mich:
 * Am besten erst alle nicht-gewollten Docks schlie??en und dann die gewollten ??ffnen.
 * Sieht im Code zwar un??bersichtlicher aus, aber sonst kann es passiern, dass im MainWindow nicht genug Platz ist.
 * Dann "explodiert" das Fenster, springt aus dem maximierten Modus und ist dann gr????er als der Bildschirm. ^^
*/

void MainWindow::actionWorkspaceTrackLayout() {
    this->addDockWidget(Qt::LeftDockWidgetArea, dwUndoView);
    this->addDockWidget(Qt::LeftDockWidgetArea, dwBusstops);
    this->splitDockWidget(dwUndoView, dwBusstops, Qt::Horizontal);
    this->tabifyDockWidget(dwBusstops, dwTours);
    this->addDockWidget(Qt::RightDockWidgetArea, dwLines);
    this->addDockWidget(Qt::RightDockWidgetArea, dwRoutes);

    dwBusstopSchedule->close();
    dwBusstopSchedule->setFloating(true);

    dwSchedule->close();
    dwSchedule->setFloating(true);

    dwTripEditor->close();
    dwTripEditor->setFloating(true);

    dwTours->close();
    dwTours->setFloating(false);

    dwTourEditor->close();
    dwTourEditor->setFloating(true);

    dwPublishedLines->close();
    dwPublishedLines->setFloating(true);

    dwBusstops->show();
    dwBusstops->setFloating(false);

    dwLines->show();
    dwLines->setFloating(false);

    dwRoutes->show();
    dwRoutes->setFloating(false);

    dwUndoView->show();
    dwUndoView->setFloating(false);

    this->resizeDocks({dwUndoView, dwBusstops}, {static_cast<int>(this->width() * 0.1), static_cast<int>(this->width() * 0.4)}, Qt::Horizontal);
    this->resizeDocks({dwBusstops, dwLines}, {static_cast<int>(this->width() * 0.4), static_cast<int>(this->width() * 0.5)}, Qt::Horizontal);
    this->resizeDocks({dwLines, dwRoutes}, {static_cast<int>(this->width() * 0.5), static_cast<int>(this->width() * 0.5)}, Qt::Vertical);

    ui->actionWorkspaceTrackLayout->setChecked(true);
    ui->actionWorkspaceBusstopSchedule->setChecked(false);
    ui->actionWorkspaceScheduling->setChecked(false);
    ui->actionWorkspaceTourPlanning->setChecked(false);
    ui->actionWorkspacePublish->setChecked(false);
}

void MainWindow::actionWorkspaceBusstopSchedule() {
    this->tabifyDockWidget(dwBusstops, dwTours);
    this->addDockWidget(Qt::LeftDockWidgetArea, dwBusstops);
    this->addDockWidget(Qt::RightDockWidgetArea, dwBusstopSchedule);

    dwLines->setFloating(false);
    dwLines->close();

    dwRoutes->setFloating(false);
    dwRoutes->close();

    dwSchedule->setFloating(true);
    dwSchedule->close();

    dwTripEditor->close();
    dwTripEditor->setFloating(true);

    dwTours->setFloating(false);
    dwTours->close();

    dwTourEditor->setFloating(true);
    dwTourEditor->close();

    dwUndoView->setFloating(true);
    dwUndoView->close();

    dwPublishedLines->setFloating(true);
    dwPublishedLines->close();

    dwBusstops->show();
    dwBusstops->setFloating(false);

    dwBusstopSchedule->show();
    dwBusstopSchedule->setFloating(false);

    this->resizeDocks({dwBusstops, dwBusstopSchedule}, {static_cast<int>(this->width() * 0.4), static_cast<int>(this->width() * 0.6)}, Qt::Horizontal);

    ui->actionWorkspaceTrackLayout->setChecked(false);
    ui->actionWorkspaceBusstopSchedule->setChecked(true);
    ui->actionWorkspaceScheduling->setChecked(false);
    ui->actionWorkspaceTourPlanning->setChecked(false);
    ui->actionWorkspacePublish->setChecked(false);
}

void MainWindow::actionWorkspaceScheduling() {
    this->addDockWidget(Qt::LeftDockWidgetArea, dwLines);
    this->addDockWidget(Qt::RightDockWidgetArea, dwSchedule);
    this->addDockWidget(Qt::RightDockWidgetArea, dwTripEditor);
    this->addDockWidget(Qt::LeftDockWidgetArea, dwUndoView);
    this->addDockWidget(Qt::BottomDockWidgetArea, dwTourEditor);
    this->splitDockWidget(dwSchedule, dwTripEditor, Qt::Horizontal);
    this->tabifyDockWidget(dwLines, dwBusstops);
    this->tabifyDockWidget(dwBusstops, dwTours);
    this->tabifyDockWidget(dwSchedule, dwRoutes);


    dwBusstops->setFloating(false);
    dwBusstops->close();

    dwBusstopSchedule->setFloating(true);
    dwBusstopSchedule->close();

    dwRoutes->setFloating(false);
    dwRoutes->close();

    dwTours->setFloating(false);
    dwTours->close();

    dwTourEditor->setFloating(true);
    dwTourEditor->close();

    dwPublishedLines->setFloating(true);
    dwPublishedLines->close();

    dwLines->show();
    dwLines->setFloating(false);

    dwSchedule->show();
    dwSchedule->setFloating(false);

    dwTripEditor->show();
    dwTripEditor->setFloating(false);

    dwUndoView->show();
    dwUndoView->setFloating(false);

    this->resizeDocks({dwLines, dwSchedule}, {static_cast<int>(this->width() * 0.01), static_cast<int>(this->width() * 0.99)}, Qt::Horizontal);
    this->resizeDocks({dwSchedule, dwTripEditor}, {static_cast<int>(this->width() * 0.9), static_cast<int>(this->width() * 0.1)}, Qt::Horizontal);
    this->resizeDocks({dwLines, dwUndoView}, {static_cast<int>(this->height() * 0.8), static_cast<int>(this->height() * 0.2)}, Qt::Vertical);

    ui->actionWorkspaceTrackLayout->setChecked(false);
    ui->actionWorkspaceBusstopSchedule->setChecked(false);
    ui->actionWorkspaceScheduling->setChecked(true);
    ui->actionWorkspaceTourPlanning->setChecked(false);
    ui->actionWorkspacePublish->setChecked(false);
}

void MainWindow::actionWorkspaceTourPlanning() {
    this->addDockWidget(Qt::LeftDockWidgetArea, dwTours);
    this->addDockWidget(Qt::RightDockWidgetArea, dwTourEditor);
    this->addDockWidget(Qt::LeftDockWidgetArea, dwUndoView);
    this->tabifyDockWidget(dwTours, dwLines);
    this->tabifyDockWidget(dwLines, dwBusstops);
    this->tabifyDockWidget(dwTourEditor, dwSchedule);

    dwBusstops->setFloating(false);
    dwBusstops->close();

    dwBusstopSchedule->setFloating(true);
    dwBusstopSchedule->close();

    dwLines->setFloating(false);
    dwLines->close();

    dwRoutes->setFloating(true);
    dwRoutes->close();

    dwSchedule->setFloating(false);
    dwSchedule->close();

    dwTripEditor->close();
    dwTripEditor->setFloating(true);

    dwPublishedLines->setFloating(true);
    dwPublishedLines->close();

    dwTours->show();
    dwTours->setFloating(false);

    dwTourEditor->show();
    dwTourEditor->setFloating(false);

    dwUndoView->show();
    dwUndoView->setFloating(false);

    this->resizeDocks({dwTours, dwTourEditor}, {static_cast<int>(this->width() * 0.5), static_cast<int>(this->width() * 0.5)}, Qt::Horizontal);
    this->resizeDocks({dwTours, dwUndoView}, {static_cast<int>(this->width() * 0.8), static_cast<int>(this->width() * 0.2)}, Qt::Vertical);

    ui->actionWorkspaceTrackLayout->setChecked(false);
    ui->actionWorkspaceBusstopSchedule->setChecked(false);
    ui->actionWorkspaceScheduling->setChecked(false);
    ui->actionWorkspaceTourPlanning->setChecked(true);
    ui->actionWorkspacePublish->setChecked(false);
}

void MainWindow::actionWorkspacePublish() {
    this->addDockWidget(Qt::RightDockWidgetArea, dwPublishedLines);
    this->addDockWidget(Qt::LeftDockWidgetArea, dwBusstops);
    this->tabifyDockWidget(dwBusstops, dwLines);

    dwLines->setFloating(false);
    dwLines->close();

    dwRoutes->setFloating(true);
    dwRoutes->close();

    dwSchedule->setFloating(true);
    dwSchedule->close();

    dwTripEditor->close();
    dwTripEditor->setFloating(true);

    dwTours->setFloating(true);
    dwTours->close();

    dwTourEditor->setFloating(true);
    dwTourEditor->close();

    dwUndoView->setFloating(true);
    dwUndoView->close();

    dwBusstops->close();
    dwBusstops->setFloating(false);

    dwBusstopSchedule->close();
    dwBusstopSchedule->setFloating(true);

    dwPublishedLines->show();
    dwPublishedLines->setFloating(false);

    ui->actionWorkspaceTrackLayout->setChecked(false);
    ui->actionWorkspaceBusstopSchedule->setChecked(false);
    ui->actionWorkspaceScheduling->setChecked(false);
    ui->actionWorkspaceTourPlanning->setChecked(false);
    ui->actionWorkspacePublish->setChecked(true);
}

void MainWindow::actionOpenBusstopSchedule(Busstop *b, QList<Route *> routes, DayType *dayType) {
    wdgBusstopSchedule->setAll(b, routes, dayType);
    dwBusstopSchedule->show();
}

void MainWindow::actionOpenTour(Tour *o) {
    wdgTours->setCurrentTour(o);
    wdgTourEditor->setCurrentTour(o);
    dwTourEditor->show();
}

void MainWindow::setUndoEnabled(bool b) {
    ui->actionUndo->setEnabled(b);
}

void MainWindow::setRedoEnabled(bool b) {
    ui->actionRedo->setEnabled(b);
}

void MainWindow::setSaved(bool b) {
    if(!b)
        QMainWindow::setWindowTitle("* ScheduleMaster");
    else
        QMainWindow::setWindowTitle("ScheduleMaster");

    ui->actionFileSave->setDisabled(b);
}

bool MainWindow::openFile(QString path) {
    ui->statusbar->showMessage(tr("Opening project file..."));

    QFile f(path);
    QFileInfo fi(path);

    if(!f.exists()) {
        QMessageBox::warning(this, tr("File not found"), tr("<p>The given file was not found!</p>"));
        qWarning() << "File \"" << path << "\" was not found!";
        actionFileOpen();
        return false;
    }

    QTextStream s(&f);

    if(!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Failed reading file"), tr("<p><b>Could not read file:</b></p><p>%1</p>").arg(fi.fileName()));
        qWarning() << "Failed reading file \"" << path << "\"!";
        actionFileOpen();
        return false;
    }

    f.close();

    qDebug() << "loading file handler...";
    fileHandler->show();
    qApp->processEvents();
    fileHandler->readFromFile(path);

    qDebug() << "refreshing ui...";
    wdgBusstops->refresh();
    wdgLines->refresh();
    wdgTours->refreshTourList();
    wdgSchedule->refreshDayTypes();
    wdgPublishedLines->refreshLineList();
    wdgPublishedLines->refreshDayTypes();
    wdgPublishedLines->refreshRouteList();

    undoStack->clear();
    knownFile = true;
    projectData->setFilePath(path);

    ui->statusbar->showMessage(path);

    LocalConfig::addLastUsedFile(path);

    return true;
}

bool MainWindow::saveFile(QString path) {
    QFile f(path);
    QFileInfo fi(path);
    QTextStream s(&f);


    if(!f.open(QIODevice::WriteOnly)) {
        if(knownFile)
            QMessageBox::warning(this, tr("Failed writing file"), tr("<p><b>Could not write to file:</b></p><p>%1</p>").arg(fi.fileName()));
        actionFileSaveAs();
        return false;
    }

    f.close();

    //projectData->cleanup();

    fileHandler->saveToFile(path);

    undoStack->setClean();
    knownFile = true;
    ui->statusbar->showMessage(tr("File saved!"), 5000);
    return true;
}

/*void MainWindow::reviceDataFromFileHandler() {

}*/

/*void MainWindow::on_actionBusstopExportList_triggered()
{
    QString plainText, csvText, htmlText;

    QFileInfo fi(this->projectFilePath);

    csvText = "Name;Important;used by lines\r\n";

    htmlText = "<!DOCTYPE HTML>\r\n<html>\r\n\t<head><title>All busstops of project " + fi.baseName() + "</title></head>\r\n\t<body style=\"font-family:sans-serif\">\r\n\t\t<table cellspacing=\"2.5\">\r\n\t\t\t<thead><tr><th>Name</th><th>Used by lines</th></tr></thead>\r\n\t\t\t\t<tbody>\r\n";

    for(int i = 0; i < projectData->getBusstopCount(); i++) {
        Busstop *b = projectData->getBusstopAt(i);
        QString name = b->name();
        bool important = b->isImportant();
        QString importantStr = important ? "Yes" : "No";
        QStringList usedLinesStrList;
        QList<Line *> usedLines = projectData->getLinesAtBusstop(b);
        for(int i = 0; i < usedLines.count(); i++)
            usedLinesStrList << "[" + usedLines[i]->name() + "]";
        QString usedLinesStr = usedLinesStrList.join(", ");
        plainText += name + "\r\n";

        csvText += "\"" + name + "\";" + importantStr + ";" + usedLinesStr + "\r\n";
        if(important)
            htmlText += "\t\t\t\t\t<tr><td><b>" + name + "</b></td><td>" + usedLinesStr + "</td></tr>\r\n";
        else
            htmlText += "\t\t\t\t\t<tr><td>" + name + "</td><td>" + usedLinesStr + "</td></tr>\r\n";
    }

    htmlText += "\t\t\t</tbody>\r\n\t\t</table>\r\n\t</body>\r\n</html>";


    DataExporter dlg;
    dlg.setFileName("all_busstops");
    dlg.setFolderName(fi.baseName());
    dlg.setText(plainText, csvText, htmlText);

    dlg.exec();
}

void MainWindow::on_actionLineExportList_triggered()
{
    QString plainText, csvText, htmlText;

    QFileInfo fi(this->projectFilePath);

    csvText = "Name;Description\r\n";

    htmlText = "<!DOCTYPE HTML>\r\n<html>\r\n\t<head><title>All lines of project " + fi.baseName() + "</title></head>\r\n\t<body style=\"font-family:sans-serif\">\r\n\t\t<table cellspacing=\"2.5\">\r\n\t\t\t<thead><tr><th>Name</th><th>Description</th></tr></thead>\r\n\t\t\t\t<tbody>\r\n";

    for(int i = 0; i < projectData->getLineCount(); i++) {
        Line *l = projectData->getLineAt(i);
        QString name = l->name();
        QColor color = l->getColor();
        QColor contrastColor = global::getContrastColor(color);
        QString description = l->getDescription();

        plainText += "[" + name + "] " + description + "\r\n";
        csvText += "\"" + name + "\";\"" + description + "\"\r\n";
        htmlText += "\t\t\t\t\t<tr><td style=\"color: " + contrastColor.name(QColor::HexRgb) + "; background-color: " + color.name(QColor::HexRgb) + "\"><b>" + name + "</b></td><td>" + description + "</td></tr>\r\n";
    }

    htmlText += "\t\t\t</tbody>\r\n\t\t</table>\r\n\t</body>\r\n</html>";


    DataExporter dlg;
    dlg.setFolderName(fi.baseName());
    dlg.setFileName("all_lines");
    dlg.setText(plainText, csvText, htmlText);

    dlg.exec();
}*/


void MainWindow::on_actionCleanup_and_troubleshooting_triggered()
{
    Troubleshooter dlg;
    dlg.exec();
}


void MainWindow::on_actionView_As_Tree_triggered() {
    ProjectTreeViewer dlg(this, projectData);
    dlg.exec();
}

void MainWindow::on_actionHelpAbout_triggered() {
    QString text;
    text = tr("<h1>About ScheduleMaster 2</h1><table><tr><td><b>Version:</b></td><td>%1</td></tr><tr><td><b>Release type:</b></td><td>Beta</td></tr></table>")
               .arg(GlobalConfig::currentVersion());

    QMessageBox msg;
    msg.setWindowTitle(tr("ScheduleMaster 2 - About"));
    msg.setText(text);
    msg.setIconPixmap(QPixmap(":/appIcon/Icon_128px.ico"));
    msg.setStandardButtons(QMessageBox::Close);
    msg.exec();
}


void MainWindow::on_actionEditProjectSettings_triggered() {
    QList<DayType> dayTypes;
    for(int i = 0; i < projectData->projectSettings()->dayTypeCount(); i++)
        dayTypes << *projectData->projectSettings()->dayTypeAt(i);

    DlgProjectSettings *dlg = new DlgProjectSettings(this);
    dlg->setNames(projectData->projectSettings()->displayName(), projectData->projectSettings()->shortName());
    dlg->setIcon(projectData->projectSettings()->icon());
    dlg->setDayTypes(dayTypes);

    dlg->exec();

    if(dlg->result() != QDialog::Accepted)
        return;

    ProjectSettings newS(nullptr);
    newS.setNames(dlg->displayName(), dlg->shortName());
    newS.setIcon(dlg->icon());
    newS.setDayTypes(dlg->dayTypes());

    undoStack->push(new CmdEditProjectSettings(projectData->projectSettings(), newS));
    wdgSchedule->refreshDayTypes();
}


void MainWindow::on_actionEditPreferences_triggered() {
    DlgPreferences* dlg = new DlgPreferences(this);
    dlg->exec();
}

/*
void MainWindow::on_actionPublishTest_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "", "D:/cpp_Projekte_Qt/ScheduleMaster/build-ScheduleMaster-Desktop_Qt_6_5_2_MinGW_64_bit-Release/Test.pdf", "Portable document format (*.pdf)");
    if(fileName.isEmpty())
        return;

    QString fileName = "D:/cpp_Projekte_Qt/ScheduleMaster/build-ScheduleMaster-Desktop_Qt_6_5_2_MinGW_64_bit-Release/Test.pdf";
    QFile f(fileName);
    if(!f.open(QFile::WriteOnly))
        return;

    QPdfWriter pdf(fileName);

    pdf.setResolution(254);
    pdf.setCreator("ScheduleMaster 2");
    pdf.setTitle("Test document");
    pdf.setPageLayout(QPageLayout(QPageSize::A4, QPageLayout::Landscape, QMargins(10, 10, 10, 10), QPageLayout::Millimeter));

    QPen defaultPen(Qt::black, 5);

    QBrush grayRow(Qt::lightGray);
    QBrush whiteRow(Qt::white);

    QFont headline("Helvetica", 16);
    headline.setBold(true);

    QFont normal("Helvetica", 11);

    defaultPen.setJoinStyle(Qt::MiterJoin);

    QPainter painter(&pdf);
    painter.setFont(headline);
    painter.setPen(defaultPen);

    QRect rect(0, 0, 2770, 75);

    // headline
    painter.drawRect(rect);
    painter.drawText(rect.adjusted(10, 10, -10, -10), Qt::AlignLeft|Qt::AlignVCenter, "21/22 Kaisersee - Grundorf - Einsteindorf - Hertz - Nordspitze - Kirchdorf");
    painter.setFont(normal);
    painter.drawText(rect.adjusted(10, 10, -10, -10), Qt::AlignRight|Qt::AlignBottom, "G??tlig ab: 07.11.2023");


    // table
    painter.setBackground(grayRow);

    for(int i = 0; i < 26; i++) {

        QRect rect(0, 200 + (i * 45), 2770, 45);
        if(i % 2 == 0) {
            painter.setBrush(grayRow);
        } else {
            painter.setBrush(whiteRow);
        }

        painter.setPen(Qt::NoPen);
        painter.drawRect(rect);

        painter.setPen(defaultPen);
        painter.drawText(rect.adjusted(5, 5, -5, -5), Qt::AlignLeft|Qt::AlignVCenter, "Haltestellenname");
    }

    QRect tableBorder(0, 200, 2770, 45*26);
    painter.setPen(defaultPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(tableBorder.adjusted(- defaultPen.width() / 2, - defaultPen.width() / 2, defaultPen.width() / 2, defaultPen.width() / 2));


    QRect footer(0, 1900 - 45 + 25, 2770, 45);
    painter.drawText(footer, Qt::AlignRight|Qt::AlignBottom, "Seite 1 von 1");

    QDesktopServices::openUrl(QUrl(fileName));
}*/



void MainWindow::on_actionPublishAll_triggered() {
    pdfExporter->exportAllLineSchedules();
    pdfExporter->exportAllTours();
}


void MainWindow::on_actionPublishManageFootnotes_triggered() {
    DlgManageFootnotes *dlg = new DlgManageFootnotes(this, projectData);
    dlg->exec();
}


void MainWindow::on_actionHelpManual_triggered() {
    QString openURL;
    if(LocalConfig::language() == LocalConfig::LanguageGerman)
        openURL = "Manual_DE.pdf";
    else
        openURL = "Manual_EN.pdf";

    if(!QFile::exists(openURL)) {
        QMessageBox::critical(this, tr("Manual not found"), tr("<p>The file was not found:</p><p><b>%1</b></p>").arg(openURL));
        return;
    }
    QDesktopServices::openUrl(QUrl(openURL));
}


void MainWindow::on_actionViewToolbarGeneral_triggered() {
    tbGeneral->setVisible(ui->actionViewToolbarGeneral->isChecked());
}


void MainWindow::on_actionViewToolbarDocks_triggered() {
    tbDocks->setVisible(ui->actionViewToolbarDocks->isChecked());
}


void MainWindow::on_actionViewToolbarWorkspaces_triggered() {
    tbWorkspaces->setVisible(ui->actionViewToolbarWorkspaces->isChecked());
}
