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

#include <QStyleHints>


#include "Commands/Cmdgeneral.h"
#include "Mainwindow.h"

#include "localconfig.h"
#include "AppInfo.h"

#include "ui_Mainwindow.h"

//#include "Widgets/wdgschedule.h"

#include "Dialogs/DlgStartupdialog.h"
#include "Dialogs/DlgProjecttreeviewer.h"
#include "Dialogs/DlgTroubleshooter.h"
#include "Dialogs/DlgProjectsettings.h"
#include "Dialogs/DlgPreferences.h"
//#include "Widgets/WdgFootnotes.h"
#include "Dialogs/DlgOmsiImport.h"
#include "Dialogs/DlgChangelog.h"


//********************************************************************************************************************************************************************
// Constructor, Destructor, Overwrites (CloseEvent e.g.)
//********************************************************************************************************************************************************************

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _projectData(new ProjectData(this)),
    fileHandler(new FileHandler(this)),
    startupDialog(new StartupDialog(parent)),
    progressLogger(new DlgProgressLogger(this)),
    pdfExporter(new DlgPdfExporter(this, _projectData)),
    knownFile(false)
{
    pdfExporter->setModal(true);
    startupDialog->setModal(true);

    qInfo() << "initializing splashscreen...";

    // init splashscreen
    QColor messageColor = QColor(0, 0, 0);
    QSplashScreen splashScreen;
    QString imagePath = ":/splashscreen/slpashscreen_light.png";
    if(qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
        imagePath = ":/splashscreen/splashscreen_dark.png";
        messageColor = QColor(255, 255, 255);
    }

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
    splashScreen.show();

    // detect crash
    if(LocalConfig::crashDetected()) {
        bool logfileSaved = QFile::exists(LocalConfig::lastLogfileName());
        qInfo() << "crash detected" + (logfileSaved ? ", logfile saved separately: " + LocalConfig::lastLogfileName() : "");
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

    connect(fileHandler, &FileHandler::actionStarted, progressLogger, &DlgProgressLogger::addEntry);
    connect(fileHandler, &FileHandler::finished, this, &MainWindow::handleFileHandlerResult);

    connect(fileHandler, &FileHandler::openFileError, this, &MainWindow::handleFileHandlerFileError);
    connect(fileHandler, &FileHandler::saveFileError, this, &MainWindow::handleFileHandlerFileError);

    connect(fileHandler, &FileHandler::unknownVersionDetected, this, &MainWindow::handleFileHandlerFileUnkownVersionWarning);
    connect(fileHandler, &FileHandler::oldVersionDetected, this, &MainWindow::handleFileHandlerFileOldVersionWarning);

    connect(_projectData, &ProjectData::loadingProgressMaxValue, progressLogger, &DlgProgressLogger::setProgressMaximum);
    connect(_projectData, &ProjectData::loadingProgressUpdated, progressLogger, &DlgProgressLogger::setProgressValue);
    connect(_projectData, &ProjectData::loadingProgressTextUpdated, progressLogger, [&](const int &type, const QString &message, const bool &showAsCurrent = false){
        DlgProgressLogger::EntryType enumType;
        switch(type) {
            case 0: enumType = DlgProgressLogger::SuccessType;  break;
            case 1: enumType = DlgProgressLogger::InfoType;     break;
            case 2: enumType = DlgProgressLogger::WarningType;  break;
            case 3: enumType = DlgProgressLogger::ErrorType;    break;
            case 4: enumType = DlgProgressLogger::CriticalType; break;
        }

        progressLogger->addEntry(enumType, message, showAsCurrent);
    });

#ifndef QT_DEBUG
    QThread::sleep(2);
#endif

    qInfo() << "loading user interface...";
    ui->setupUi(this);

    qInfo() << "setting up dock widgets...";
    ui->centralwidget -> setHidden(true);
    dwUndoView        -> setWidget(wdgUndoView);
    dwBusstops        -> setWidget(wdgBusstops);
    dwLines           -> setWidget(wdgLines);
    dwRoutes          -> setWidget(wdgRoutes);
    dwSchedule        -> setWidget(wdgSchedule);
    dwTripEditor      -> setWidget(wdgTripEditor);
    dwBusstopSchedule -> setWidget(wdgBusstopSchedule); dwBusstopSchedule->resize(900, 831);
    dwTours           -> setWidget(wdgTours);
    dwTourEditor      -> setWidget(wdgTourEditor);
    dwPublishedLines  -> setWidget(wdgPublishedLines);
    dwFootnotes       -> setWidget(wdgFootnotes);

    actionWorkspaceTrackLayout();

    qInfo() << "loading menubar...";
    // load menubar
    splashScreen.showMessage(tr("loading menubar..."), Qt::AlignBottom, messageColor);

    qDebug() << "loading toggleViewActions...";
    QAction *actDockUndoView        = dwUndoView        -> toggleViewAction();
    QAction *actDockBusstops        = dwBusstops        -> toggleViewAction();
    QAction *actDockLines           = dwLines           -> toggleViewAction();
    QAction *actDockRoutes          = dwRoutes          -> toggleViewAction();
    QAction *actDockSchedule        = dwSchedule        -> toggleViewAction();
    QAction *actDockBusstopSchedule = dwBusstopSchedule -> toggleViewAction();
    QAction *actDockTours           = dwTours           -> toggleViewAction();
    QAction *actDockTourEditor      = dwTourEditor      -> toggleViewAction();
    QAction *actDockPublishedLines  = dwPublishedLines  -> toggleViewAction();
    QAction *actDockFootnotes       = dwFootnotes       -> toggleViewAction();

    qDebug() << "setting icons of toggleViewActions...";
    actDockUndoView        -> setIcon(QIcon(":/icons/UndoRedo.ico"));
    actDockBusstops        -> setIcon(QIcon(":/icons/Busstop.ico"));
    actDockLines           -> setIcon(QIcon(":/icons/Line.ico"));
    actDockRoutes          -> setIcon(QIcon(":/icons/Route.ico"));
    actDockSchedule        -> setIcon(QIcon(":/icons/Schedule.ico"));
    actDockBusstopSchedule -> setIcon(QIcon(":/icons/BusstopSchedule.ico"));
    actDockTours           -> setIcon(QIcon(":/icons/TourList.ico"));
    actDockTourEditor      -> setIcon(QIcon(":/icons/Tour.ico"));
    actDockPublishedLines  -> setIcon(QIcon(":/icons/PublishedLines.ico"));
    actDockFootnotes       -> setIcon(QIcon(":/icons/Footnote.ico"));

    qDebug() << "setting shortcuts toggleViewActions...";
    actDockUndoView        -> setShortcut(QKeySequence(tr("Alt+U")));
    actDockBusstops        -> setShortcut(QKeySequence(tr("Alt+B")));
    actDockLines           -> setShortcut(QKeySequence(tr("Alt+L")));
    actDockRoutes          -> setShortcut(QKeySequence(tr("Alt+R")));
    actDockSchedule        -> setShortcut(QKeySequence(tr("Alt+S")));
    actDockBusstopSchedule -> setShortcut(QKeySequence(tr("Alt+Shift+B")));
    actDockTours           -> setShortcut(QKeySequence(tr("Alt+T")));
    actDockTourEditor      -> setShortcut(QKeySequence(tr("Alt+Shift+T")));
    actDockFootnotes       -> setShortcut(QKeySequence(tr("Alt+F")));

    ui->menuBusstops->addActions(wdgBusstops->actions());
    ui->menuLines   ->addActions(wdgLines   ->actions());
    ui->menuRoutes  ->addActions(wdgRoutes  ->actions());
    ui->menuTours   ->addActions(wdgTours   ->actions());

    qDebug() << "adding toggleViewActions to menubar";
    ui->menuDocks->addAction(actDockBusstops);
    ui->menuDocks->addAction(actDockLines);
    ui->menuDocks->addAction(actDockRoutes);
    ui->menuDocks->addAction(actDockSchedule);
    ui->menuDocks->addAction(actDockBusstopSchedule);
    ui->menuDocks->addAction(actDockTours);
    ui->menuDocks->addAction(actDockTourEditor);
    ui->menuDocks->addSeparator();
    ui->menuDocks->addAction(actDockPublishedLines);
    ui->menuDocks->addAction(actDockFootnotes);
    ui->menuDocks->addSeparator();
    ui->menuDocks->addAction(actDockUndoView);

    qDebug() << "loading undo and redo action...";
    undoAction = undoStack()->createUndoAction(this, tr("Undo"));
    redoAction = undoStack()->createRedoAction(this, tr("Redo"));
    undoAction->setIcon(QIcon(":/icons/Undo.ico"));
    redoAction->setIcon(QIcon(":/icons/Redo.ico"));
    ui->menuEdit->addActions({undoAction, redoAction});
    ui->menuEdit->addSeparator();
    ui->menuEdit->addActions({ui->actionEditProjectSettings});
    ui->menuEdit->addSeparator();
    ui->menuEdit->addActions({ui->actionEditPreferences}),

    qInfo() << "loading shortcuts";
    splashScreen.showMessage(tr("loading shortcuts..."), Qt::AlignBottom, messageColor);
    ui->actionFileNew->setShortcut(QKeySequence::New);
    ui->actionFileOpen->setShortcut(QKeySequence::Open);
    ui->actionFileSave->setShortcut(QKeySequence::Save);
    ui->actionFileSaveAs->setShortcuts({QKeySequence(Qt::CTRL|Qt::SHIFT|Qt::Key_S), QKeySequence::SaveAs});
    ui->actionFileClose->setShortcuts({QKeySequence(Qt::CTRL|Qt::Key_W), QKeySequence::Close});
    ui->actionFileQuit->setShortcuts({QKeySequence(Qt::CTRL|Qt::Key_Q), QKeySequence::Quit});

    undoAction->setShortcut(QKeySequence::Undo);
    redoAction->setShortcut(QKeySequence::Redo);
    ui->actionEditProjectSettings->setShortcut(QKeySequence(Qt::CTRL|Qt::SHIFT|Qt::Key_Comma));
    ui->actionEditPreferences->setShortcuts({QKeySequence(Qt::CTRL|Qt::Key_Comma), QKeySequence::Preferences});

    qInfo() << "loading toolbars...";
    splashScreen.showMessage(tr("loading toolbars..."), Qt::AlignBottom, messageColor);

    tbGeneral    = new QToolBar(tr("general"),    this);
    tbDocks      = new QToolBar(tr("docks"),      this);
    tbWorkspaces = new QToolBar(tr("workspaces"), this);

    qDebug() << "\tgeneral toolbar";
    tbGeneral->setToolButtonStyle(Qt::ToolButtonIconOnly);
    tbGeneral->addAction(ui->actionFileNew);
    tbGeneral->addAction(ui->actionFileOpen);
    tbGeneral->addAction(ui->actionFileSave);
    tbGeneral->addSeparator();
    tbGeneral->addAction(undoAction);
    tbGeneral->addAction(redoAction);
    tbGeneral->addSeparator();
    tbGeneral->addAction(ui->actionEditProjectSettings);
    tbGeneral->addAction(ui->actionEditPreferences);

    qDebug() << "\tdocks toolbar";
    tbDocks->setToolButtonStyle(Qt::ToolButtonIconOnly);
    tbDocks->addAction(actDockBusstops);
    tbDocks->addAction(actDockLines);
    tbDocks->addAction(actDockRoutes);
    tbDocks->addAction(actDockSchedule);
    tbDocks->addAction(actDockBusstopSchedule);
    tbDocks->addAction(actDockTours);
    tbDocks->addAction(actDockTourEditor);
    tbDocks->addAction(actDockPublishedLines);
    tbDocks->addAction(actDockFootnotes);
    tbDocks->addSeparator();
    tbDocks->addAction(actDockUndoView);

    toolBarSpacer1 = new QWidget(this);
    toolBarSpacer2 = new QWidget(this);

    qDebug() << "\tworkspaces toolbar";
    tbWorkspaces->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    tbWorkspaces->addWidget(toolBarSpacer1);
    tbWorkspaces->addAction(ui->actionWorkspaceRouting);
    tbWorkspaces->addAction(ui->actionWorkspaceScheduling);
    tbWorkspaces->addAction(ui->actionWorkspaceTours);
    tbWorkspaces->addAction(ui->actionWorkspacePublish);
    tbWorkspaces->addWidget(toolBarSpacer2);

    tbWorkspaces->setStyleSheet(QString("QToolButton { width: 100px; }"));

    addToolBar(Qt::TopToolBarArea,    tbGeneral);
    addToolBar(Qt::TopToolBarArea,    tbDocks);
    updateWorkspacesToolbarPosition();

    ui->actionViewToolbarGeneral    ->setChecked(true);
    ui->actionViewToolbarDocks      ->setChecked(true);
    ui->actionViewToolbarWorkspaces ->setChecked(true);

    qInfo() << "loading signals and slots...";
    splashScreen.showMessage(tr("loading signals and slots..."), Qt::AlignBottom, messageColor);

    connect(wdgBusstops, &WdgBusstops::busstopScheduleRequested, this, &MainWindow::actionOpenBusstopSchedule);

    QList<QAction *> scheduleActions = wdgSchedule->actions();
    ui->menuSchedule->addActions(scheduleActions);

    // FÜR DEN UMBAU/REWORK WICHTIG
    // Wenn WdgTripEditor auch überarbeitet wird, am besten die ScheduleActions mit den jeweiligen WdgTripEditor-Actions (Slot trigger()) verbinden, die Aktions-Funktionen werden dann ja nicht mehr existieren :)
    connect(scheduleActions[0], &QAction::triggered, wdgTripEditor, &WdgTripEditor::actionNew);
    connect(scheduleActions[1], &QAction::triggered, wdgTripEditor, &WdgTripEditor::actionCopy);
    connect(scheduleActions[2], &QAction::triggered, wdgTripEditor, &WdgTripEditor::actionDelete);

    QMenu *hourBreakMenu = new QMenu(tr("Hour Break"), ui->menuSchedule);
    hourBreakMenu->setEnabled(false);
    hourBreakMenu->setIcon(QIcon(":/icons/HourBreak.ico"));
    ui->menuSchedule->addSeparator();
    ui->menuSchedule->addMenu(hourBreakMenu);

    hourBreakMenu->addActions(wdgSchedule->hourBreakActions());

    connect(wdgLines, &WdgLines::currentLineChanged, hourBreakMenu, &QMenu::setEnabled);

    qDebug() << "\tfile operations";
    connect(ui->actionFileNew,                     &QAction::triggered,                    this,               &MainWindow::actionFileNew);
    connect(ui->actionFileOpen,                    &QAction::triggered,                    this,               [this](){actionFileOpen();});
    connect(ui->actionFileSave,                    &QAction::triggered,                    this,               &MainWindow::actionFileSave);
    connect(ui->actionFileSaveAs,                  &QAction::triggered,                    this,               &MainWindow::actionFileSaveAs);
    connect(ui->actionFileClose,                   &QAction::triggered,                    this,               &MainWindow::actionFileClose);
    connect(ui->actionFileQuit,                    &QAction::triggered,                    this,               &MainWindow::actionQuit);
    connect(undoStack(),                           &QUndoStack::cleanChanged,              this,               [this](bool b){setWindowModified(!b);});
    connect(undoStack(),                           &QUndoStack::cleanChanged,              ui->actionFileSave, &QAction::setDisabled);
    connect(undoAction,                            &QAction::triggered,                    this,               &MainWindow::refreshUndo);
    connect(redoAction,                            &QAction::triggered,                    this,               &MainWindow::refreshRedo);

    qDebug() << "\tworkspace actions";
    connect(ui->actionWorkspaceRouting,            &QAction::triggered,                    this,               &MainWindow::actionWorkspaceTrackLayout);
    connect(ui->actionWorkspaceScheduling,         &QAction::triggered,                    this,               &MainWindow::actionWorkspaceScheduling);
    connect(ui->actionWorkspaceTours,              &QAction::triggered,                    this,               &MainWindow::actionWorkspaceTourPlanning);
    connect(ui->actionWorkspacePublish,            &QAction::triggered,                    this,               &MainWindow::actionWorkspacePublish);

    qDebug() << "\tdock-to-dock actions";
    connect(wdgBusstops,                           &WdgBusstops::currentBusstopChanged,    wdgBusstopSchedule, &WdgBusstopSchedule::setBusstop);
    connect(wdgLines,                              &WdgLines::currentLineChanged,          wdgRoutes,          &WdgRoutes::setCurrentLine);
    connect(wdgLines,                              &WdgLines::currentLineChanged,          wdgSchedule,        &WdgSchedule::setCurrentLine);
    connect(wdgSchedule,                           &WdgSchedule::busstopScheduleRequested, this,               &MainWindow::actionOpenBusstopSchedule);
    connect(wdgSchedule,                           &WdgSchedule::tourRequested,            this,               &MainWindow::actionOpenTour);
    connect(wdgSchedule,                           &WdgSchedule::currentLineChanged,       wdgTripEditor,      &WdgTripEditor::setCurrentLine);
    connect(wdgSchedule,                           &WdgSchedule::currentDayTypeChanged,    wdgTripEditor,      &WdgTripEditor::setCurrentDayType);
    connect(wdgSchedule,                           &WdgSchedule::currentTripsChanged,      wdgTripEditor,      &WdgTripEditor::setCurrentTrips);
    connect(wdgTripEditor,                         &WdgTripEditor::tripsChanged,           wdgSchedule,        &WdgSchedule::setCurrenTrips);
    connect(wdgTours,                              &WdgTours::currentTourChanged,          wdgTourEditor,      &WdgTourEditor::setCurrentTour);

    qDebug() << "\tundo/redo refreshs";
    // Busstops
    /*connect(wdgBusstops,                           &WdgBusstops::refreshRequested,         wdgBusstops,        &WdgBusstops::refreshBusstopList);
    connect(wdgBusstops,                           &WdgBusstops::refreshRequested,         wdgSchedule,        &WdgSchedule::refreshSchedule);
    connect(wdgBusstops,                           &WdgBusstops::refreshRequested,         wdgRoutes,          &WdgRoutes::refreshRouteList);*/

    // Lines
    /*connect(wdgLines,                              &WdgLines::refreshRequested,            wdgLines,           &WdgLines::refreshLineList);
    connect(wdgLines,                              &WdgLines::refreshRequested,            wdgSchedule,        &WdgSchedule::refreshSchedule);
    connect(wdgLines,                              &WdgLines::refreshRequested,            wdgPublishedLines,  &WdgPublishedLines::refreshRoutes);*/

    // Routes
    /*connect(wdgRoutes,                             &WdgRoutes::refreshRequested,           wdgRoutes,          &WdgRoutes::refreshRouteList);
    connect(wdgRoutes,                             &WdgRoutes::refreshRequested,           wdgPublishedLines,  &WdgPublishedLines::refreshRoutes);*/


    // Tours
    // connect(wdgTours,                              &WdgTours::refreshRequested,            wdgTours,           &WdgTours::refresh);
    // connect(wdgTours,                              &WdgTours::refreshRequested,            wdgTourEditor,      &WdgTourEditor::refresh);

    // Published Lines
    connect(wdgPublishedLines,                     &WdgPublishedLines::refreshRequested,   wdgPublishedLines,  &WdgPublishedLines::refreshDayTypes);
    connect(wdgPublishedLines,                     &WdgPublishedLines::refreshRequested,   wdgPublishedLines,  &WdgPublishedLines::refreshBusstopList);

    // Footnotes
    connect(wdgFootnotes,                          &WdgFootnotes::refreshRequested,        wdgFootnotes,       &WdgFootnotes::refreshFootnotes);


    qInfo() << "loading last used files...";
    splashScreen.showMessage(tr("loading last used files..."), Qt::AlignBottom, messageColor);

    refreshLastUsedFiles();

    splashScreen.showMessage(tr("loading startup dialog..."), Qt::AlignBottom, messageColor);

    showMaximized();

    QStringList args = qApp->arguments();
    if(args.count() >= 2)
        openFile(args[1]);
    else {
        qInfo() << "loading startup dialog";
        splashScreen.close();
        startupDialog->show();
        connect(startupDialog, &QDialog::finished, this, &MainWindow::startupDialogHandler);
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

// Close Event
void MainWindow::closeEvent(QCloseEvent * event) {
    if(!actionFileClose())
        event->ignore();
    else {
        qInfo() << "closing mainWindow...";
        event->accept();
    }
}

ProjectData *MainWindow::projectData() const {
    return _projectData;
}

QUndoStack *MainWindow::undoStack() const {
    return _projectData->undoStack();
}

void MainWindow::startupDialogHandler() {
    qInfo() << "closed startupDialog";

    if(startupDialog->result() == QDialog::Accepted) {
        int action =startupDialog->getAction();
        if (action == StartupDialog::OpenFile)
            actionFileOpen();
        else if(action == StartupDialog::OpenRecentFile)
            openFile(startupDialog->getFilePath());
        else if(action == StartupDialog::Quit) {
            QApplication::quit();
        }
    }
}

//********************************************************************************************************************************************************************
// Actions
//********************************************************************************************************************************************************************

bool MainWindow::actionFileNew() {
    return actionFileClose();
}

bool MainWindow::actionFileOpen(QString path) {
    QDir dir;
    if(knownFile) {
        QFileInfo fi(_projectData->filePath());
        dir = fi.dir().absolutePath();
    } else {
        dir = LocalConfig::defaultProjectLocation();
    }

    if(!actionFileClose())
        return false;

    if(!dir.exists())
        dir.mkpath(dir.path());

    if(!QFile::exists(path)) {
        path = QFileDialog::getOpenFileName(this, "", dir.path(), tr("ScheduleMaster File (*.smp  *.json)"));
        if(path == "") {
            qWarning() << "cannot open file \"" << path << "\" - invalid path";
            return false;
        }
    }

    return openFile(path);
}

bool MainWindow::actionFileSave() {
    if(!knownFile)
        return actionFileSaveAs();

    return saveFile(_projectData->filePath());
    //return saveFile(projectFilePath);
}

bool MainWindow::actionFileSaveAs() {
    QDir dir;
    if(knownFile) {
        QFileInfo fi(_projectData->filePath());
        dir = fi.dir().absolutePath();
    } else {
        dir = LocalConfig::defaultProjectLocation();
    }

    if(!dir.exists())
        dir.mkpath(dir.path());

    QString path = QFileDialog::getSaveFileName(this, "", dir.path(), tr("ScheduleMaster File (*.smp);;JSON File (*.json)"));
    if(path == "")
        return false;

    if(saveFile(path)) {
        knownFile = true;
        //projectFilePath = path;
        _projectData->setFilePath(path);
        return true;
    }
    return false;
}

bool MainWindow::actionFileClose() {
    if(!undoStack()->isClean()) {
        QMessageBox::StandardButton msg = QMessageBox::warning(this, tr("Unsaved Changes"), tr("<p><b>There are unsaved changes!</b></p><p>Do want to save them before closing this file?</p>"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

        if(msg == QMessageBox::Yes) {
            if(!actionFileSave())
                return false;
        } else if(msg == QMessageBox::Cancel) {
            return false;
        }
    }
    qInfo() << "closing current file...";
    _projectData->reset();
    knownFile = false;
    ui->actionFileSave->setEnabled(true);

    wdgRoutes->setCurrentLine(nullptr);
    wdgSchedule->setCurrentLine(nullptr);
    // wdgTours->setCurrentTour(nullptr);
    wdgTourEditor->setCurrentTour(nullptr);
    wdgPublishedLines->setCurrentLine(nullptr);

    //wdgBusstops->refreshBusstopList();
    //wdgLines->refreshLineList();
    //wdgRoutes->refreshRouteList();
    // wdgSchedule->refreshSchedule();
    // wdgSchedule->refreshDirections();
    // wdgSchedule->refreshDayTypes();
    // wdgTours->refresh();
    wdgTourEditor->refresh();
    wdgPublishedLines->refreshDayTypes();
    wdgPublishedLines->refreshRoutes();
    return true;
}

bool MainWindow::actionQuit() {
    close();
    return true;
}

void MainWindow::refreshLastUsedFiles() {
    ui->menuOpenRecent->clear();
    _lastUsedFileNames.clear();
    _lastUsedFileActions.clear();

    QStringList lastUsedFiles = LocalConfig::lastUsedFiles();

    int i = 0;
    foreach(QString path, lastUsedFiles) {
        QFileInfo fi(path);
        QAction *action = new QAction(fi.fileName(), ui->menuOpenRecent);
        _lastUsedFileActions << action;
        _lastUsedFileNames   << path;
        ui->menuOpenRecent->addAction(action);

        connect(action, &QAction::triggered, this, [this, path](){actionFileOpen(path);});
        i++;
        if(i >= 15)
            break;
    }
}

void MainWindow::refreshUndo() {
    refreshAfterUndoRedo(dynamic_cast<const CmdAbstract *>(undoStack()->command(undoStack()->index()))->commandType());
}

void MainWindow::refreshRedo() {
    CmdType type = dynamic_cast<const CmdAbstract *>(undoStack()->command(undoStack()->index() - 1))->commandType();
    refreshAfterUndoRedo(type);
}

void MainWindow::refreshAfterUndoRedo(CmdType t) {
    qDebug() << "refreshing views because of undo/redo command. Type:" << t;

    if(t == GeneralType) {
        // wdgSchedule->refreshDayTypes();
    }
    if(t == BusstopsType) {
        //wdgBusstops->refreshBusstopList();
        // wdgSchedule->refreshSchedule();
        //wdgRoutes->refreshRouteList();
    }
    if(t == LinesType) {
        //wdgLines->refreshLineList();
        // wdgSchedule->refreshDirections();
        wdgPublishedLines->refreshRoutes();
    }
    if(t == RoutesType) {
        //wdgRoutes->refreshRouteList();
        wdgPublishedLines->refreshRoutes();
    }
    if(t == ScheduleType) {
        // wdgSchedule->refreshSchedule();
    }
    if(t == ScheduleHourBreakType) {
        // wdgSchedule->refreshSchedule();
        wdgSchedule->refreshHourBreak();
    }
    if(t == ToursType) {
        // wdgTours->refresh();
        wdgTourEditor->refresh();
    }
    if(t == PublicationsType) {
        wdgPublishedLines->refreshDayTypes();
        wdgPublishedLines->refreshBusstopList();
    }
    if(t == FootnotesType) {
        wdgFootnotes->refreshFootnotes();
        // wdgSchedule->refreshSchedule();
    }
}

void MainWindow::updateWorkspacesToolbarPosition() {
    tbWorkspaces->setMovable(false);
    tbWorkspaces->setFloatable(false);
    Qt::ToolBarArea area = LocalConfig::workspacesToolbarPosition();
    addToolBar(area, tbWorkspaces);

    if(area == Qt::TopToolBarArea) {
        tbWorkspaces->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        tbWorkspaces->setStyleSheet("");
        toolBarSpacer1->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        toolBarSpacer2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    } else {
        tbWorkspaces->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        tbWorkspaces->setStyleSheet("QToolButton { width: 100px; }");
        toolBarSpacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        toolBarSpacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    }

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

    dwFootnotes->close();
    dwFootnotes->setFloating(true);

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

    ui->actionWorkspaceRouting->setChecked(true);
    ui->actionWorkspaceScheduling->setChecked(false);
    ui->actionWorkspaceTours->setChecked(false);
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

    ui->actionWorkspaceRouting->setChecked(false);
    ui->actionWorkspaceScheduling->setChecked(true);
    ui->actionWorkspaceTours->setChecked(false);
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

    ui->actionWorkspaceRouting->setChecked(false);
    ui->actionWorkspaceScheduling->setChecked(false);
    ui->actionWorkspaceTours->setChecked(true);
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

    ui->actionWorkspaceRouting->setChecked(false);
    ui->actionWorkspaceScheduling->setChecked(false);
    ui->actionWorkspaceTours->setChecked(false);
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

bool MainWindow::openFile(QString path) {
    ui->statusbar->showMessage(tr("Opening project file..."));

    _projectData->setParent(nullptr);
    _projectData->moveToThread(fileHandler);

    fileHandler->setAction(FileHandler::ReadFileAction);
    fileHandler->setFilePath(path);

    progressLogger->setWindowTitle(tr("Reading file..."));
    progressLogger->show();

    fileHandler->start();
    return fileHandler->success();
}

bool MainWindow::saveFile(QString path) {
    ui->statusbar->showMessage(tr("Saving project file..."));

    _projectData->setParent(nullptr);
    _projectData->moveToThread(fileHandler);

    fileHandler->setAction(FileHandler::SaveFileAction);
    fileHandler->setFilePath(path);

    progressLogger->setWindowTitle(tr("Saving file..."));
    progressLogger->show();

    fileHandler->start();
    return fileHandler->success();
}

void MainWindow::handleFileHandlerFileError(const QString &filePath, const QString errorString) {
    if(fileHandler->action() == FileHandler::ReadFileAction)
        QMessageBox::warning(this, tr("Couldn't open file"), tr("<p><b>The file couldn't be opened:</b></p><p>%1</p><p>%2</p>").arg(filePath).arg(errorString));
    else if(fileHandler->action() == FileHandler::SaveFileAction)
        QMessageBox::warning(this, tr("Couldn't save file"), tr("<p><b>The file couldn't be saved:</b></p><p>%1</p><p>%2</p>").arg(filePath).arg(errorString));
}

void MainWindow::handleFileHandlerFileOldVersionWarning(AppInfo::AppVersion *version) {
    QMessageBox::warning(this, tr("File format changed"), tr("<p><b>This file was created in an older version of ScheduleMaster that used a different file format!</b></p><p>We'll try to convert it to the current format automatically but it's recommended to create a backup of your original file to avoid data loss!</p><p><table><tr><td><b>Current version:</b></td><td>%1</td></tr><tr><td><b>File version:</b></td><td>%2</td></tr></table></p>").arg(AppInfo::currentVersion()->name(), version->name()));
}

void MainWindow::handleFileHandlerFileUnkownVersionWarning() {
    QMessageBox::warning(this, tr("File format changed"), tr("<p><b>This file was created in an unkown version of ScheduleMaster!</b></p><p>We'll try to open the file anyway but it's recommended to create a backup of your original file to avoid data loss!</p>"));
}

void MainWindow::handleFileHandlerResult() {
    _projectData->setParent(this);
    if(!fileHandler->success()) {
        progressLogger->finish();
        return;
    }

    QString filePath = fileHandler->filePath();

    if(fileHandler->action() == FileHandler::ReadFileAction) {
        progressLogger->addEntry(DlgProgressLogger::InfoType, tr("Updating ui..."), true);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        qApp->processEvents();
        qDebug() << "refreshing ui...";
        // wdgTours->refresh();
        // wdgSchedule->refreshDayTypes();
        wdgPublishedLines->refreshLineList();
        wdgPublishedLines->refreshDayTypes();
        wdgPublishedLines->refreshRoutes();
        wdgFootnotes->refreshFootnotes();
        _projectData->setFilePath(filePath);
        ui->statusbar->showMessage(filePath);
        LocalConfig::addLastUsedFile(filePath);
        undoStack()->clear();
        refreshLastUsedFiles();
        QApplication::restoreOverrideCursor();
    } else if(fileHandler->action() == FileHandler::SaveFileAction) {
        undoStack()->setClean();
        ui->statusbar->showMessage(tr("File saved!"), 5000);
    }

    QString projectName = _projectData->projectSettings()->displayName();
    setWindowTitle("[*] " + projectName + " - ScheduleMaster");
    ui->actionFileSave->setEnabled(false);

    knownFile = true;
    progressLogger->finish();
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
    ProjectTreeViewer dlg(this, _projectData);
    dlg.exec();
}

void MainWindow::on_actionHelpAbout_triggered() {
    if(!AppInfo::currentVersion())
        return;

    QString text;
    text = tr("<h1>About ScheduleMaster</h1><table><tr><td><b>Version:</b></td><td>%1</td></tr><tr><td><b>Build ID:</b></td><td>%2</td></tr></table>")
               .arg(AppInfo::currentVersion()->name())
               .arg(AppInfo::currentVersion()->buildID());

    QMessageBox msg;
    msg.setWindowTitle(tr("ScheduleMaster - About"));
    msg.setText(text);
    msg.setIconPixmap(QPixmap(":/appIcon/Icon_128px.ico"));
    msg.setStandardButtons(QMessageBox::Close);
    msg.exec();
}


void MainWindow::on_actionHelpAboutQt_triggered() {
    QMessageBox::aboutQt(this, tr("About Qt"));
}



void MainWindow::on_actionEditProjectSettings_triggered() {
    DlgProjectSettings *dlg = new DlgProjectSettings(this, _projectData->projectSettings());

    dlg->exec();

    if(dlg->result() != QDialog::Accepted)
        return;

    ProjectSettings newS = dlg->projectSettings();
    undoStack()->push(new CmdEditProjectSettings(_projectData->projectSettings(), newS));
}


void MainWindow::on_actionEditPreferences_triggered() {
    DlgPreferences* dlg = new DlgPreferences(this);
    dlg->exec();
    updateWorkspacesToolbarPosition();
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
    /*WdgFootnotes *dlg = new WdgFootnotes(this, projectData, undoStack);
    dlg->exec();*/
}


void MainWindow::on_actionHelpManual_triggered() {
    QString openURL;
    if(LocalConfig::language() == LocalConfig::German)
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

void MainWindow::on_actionFileImportOmsiSchedule_triggered() {
    DlgOmsiImport dlg(this, _projectData);
    dlg.exec();
    _projectData->setParent(this);

    qDebug() << "refreshing ui...";
    //wdgBusstops->refreshBusstopList();
    //wdgLines->refreshLineList();
    // wdgTours->refresh();
    // wdgSchedule->refreshDayTypes();
    wdgPublishedLines->refreshLineList();
    wdgPublishedLines->refreshDayTypes();
    wdgPublishedLines->refreshRoutes();
}


void MainWindow::on_actionFileExportRoutesWithProfilesCsv_triggered() {
    QString path = QFileDialog::getExistingDirectory(this, "", "", QFileDialog::ShowDirsOnly);

    QDir dir(path);
    if(!dir.exists())
        return;

    for(int i = 0; i < _projectData->lineCount(); i++) {
        Line *l = _projectData->lineAt(i);
        QDir subDir = dir.path() + "/" + l->name();
        if(!subDir.exists()) {
            bool ok = dir.mkdir(l->name());
            if(!ok) {
                qInfo() << "[CSV Export] Couldn't create directory:" << l->name();
                continue;
            }
        }

        for(int j = 0; j < l->routeCount(); j++) {
            Route *r = l->routeAt(j);
            QFile f(path + "/" + l->name() + "/" + QString::number(r->code()) + "_" + r->name() + ".csv");
            if(!f.open(QFile::WriteOnly)) {
                qInfo() << "[CSV Export] Couldn't create file:" << (QString::number(r->code()) + "_" + r->name() + ".csv");
                continue;
            }
            QTextStream s(&f);
            s.setEncoding(QStringConverter::Latin1);
            s << "Name:;" << r->name() << "\r\n";
            s << "Code:;" << QString::number(r->code()) << "\r\n";
            s << "\r\n";
            s << "Busstops;";
            for(int k = 0; k < r->timeProfileCount(); k++) {
                s << r->timeProfileAt(k)->name() << ";";
            }
            s << "\r\n";

            for(int k = 0; k < r->busstopCount(); k++) {
                Busstop *b = r->busstopAt(k);
                s << b->name() << ";";
                for(int m = 0; m < r->timeProfileCount(); m++) {
                    TimeProfileItem *itm = r->timeProfileAt(m)->busstop(b);
                    if(itm)
                        s << QString::number(itm->depValue()).replace(".", ",") << ";";
                    else
                        s << "0;";
                }
                s << "\r\n";
            }

            f.close();
        }
    }
}

void MainWindow::on_actionHelpChangelog_triggered() {
    DlgChangelog dlg;
    dlg.exec();
}
